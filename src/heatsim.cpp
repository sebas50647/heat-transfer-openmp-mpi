// Copyright 2024 mas cquest que nunca
#include <omp.h>
#include <mpi.h>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

#include "HeatDistributionSimulator.hpp"
#include "Job.hpp"

namespace {

// Only primitive fields travel over MPI. Job itself holds a std::string
// and a raw double*, neither of which are meaningful once copied as raw
// bytes into another process's address space.
struct JobDescriptor {
  uint64_t rows = 0;
  uint64_t cols = 0;
  double stepDuration = 0;
  double thermalDiff = 0;
  double cellHeight = 0;
  double sensitivity = 0;
};

JobDescriptor toDescriptor(const Job &job) {
  JobDescriptor d;
  d.rows = job.getRows();
  d.cols = job.getColumns();
  d.stepDuration = job.getStepDuration();
  d.thermalDiff = job.getThermalDiffusion();
  d.cellHeight = job.getCellHeight();
  d.sensitivity = job.getSensitivity();
  return d;
}

// Greedy longest-processing-time-first assignment: bigger plates get
// placed first, always onto whichever rank currently holds the least
// total work. Every rank computes this locally from the same broadcast
// descriptors, so no extra communication is needed to agree on it.
std::vector<int> assignJobsBySize(const std::vector<JobDescriptor> &jobs,
                                   int numRanks) {
  std::vector<int> order(jobs.size());
  std::iota(order.begin(), order.end(), 0);
  std::sort(order.begin(), order.end(), [&](int a, int b) {
    return jobs[a].rows * jobs[a].cols > jobs[b].rows * jobs[b].cols;
  });

  std::vector<uint64_t> loadPerRank(numRanks, 0);
  std::vector<int> owner(jobs.size());
  for (int idx : order) {
    int lightest = static_cast<int>(
        std::min_element(loadPerRank.begin(), loadPerRank.end()) -
        loadPerRank.begin());
    owner[idx] = lightest;
    loadPerRank[lightest] += jobs[idx].rows * jobs[idx].cols;
  }
  return owner;
}

}  // namespace

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);  // Initialize the MPI environment

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get the rank of the process
  MPI_Comm_size(MPI_COMM_WORLD, &size);  // Get the number of processes

  std::vector<Job> jobs;
  int num_jobs = 0;
  int num_threads =
      omp_get_max_threads();  // Default to maximum available threads

  if (rank == 0) {
    if (argc < 2) {
      std::cout << "Usage: " << argv[0] << " <job file>\n";
      MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (argc >= 3) {
      num_threads = std::stoi(argv[2]);  // Use specified number of threads
    }

    JobReader reader(argv[1]);
    reader.read();
    jobs = reader.getJobs();
    num_jobs = jobs.size();
  }

  // Broadcast the number of jobs to all processes
  MPI_Bcast(&num_jobs, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // Broadcast just the primitive job parameters to all processes
  std::vector<JobDescriptor> descriptors(num_jobs);
  if (rank == 0) {
    for (int i = 0; i < num_jobs; i++) descriptors[i] = toDescriptor(jobs[i]);
  }
  MPI_Bcast(descriptors.data(),
            static_cast<int>(num_jobs * sizeof(JobDescriptor)), MPI_BYTE, 0,
            MPI_COMM_WORLD);

  std::vector<int> ownerOfJob = assignJobsBySize(descriptors, size);

  // Send each plate only to the rank that will simulate it, instead of
  // broadcasting every matrix to every process.
  const int kMatrixTag = 0;
  std::vector<double *> localMatrix(num_jobs, nullptr);
  for (int i = 0; i < num_jobs; i++) {
    int cells = static_cast<int>(descriptors[i].rows * descriptors[i].cols);
    int owner = ownerOfJob[i];

    if (rank == 0 && owner == 0) {
      localMatrix[i] = jobs[i].getTemperatures();
    } else if (rank == 0) {
      MPI_Send(jobs[i].getTemperatures(), cells, MPI_DOUBLE, owner,
                kMatrixTag, MPI_COMM_WORLD);
    } else if (rank == owner) {
      localMatrix[i] = new double[cells];
      MPI_Recv(localMatrix[i], cells, MPI_DOUBLE, 0, kMatrixTag,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  }

  std::vector<int> steps(num_jobs, 0);
  std::vector<int> timeTaken(num_jobs, 0);

  for (int i = 0; i < num_jobs; i++) {
    if (ownerOfJob[i] != rank) continue;

    const JobDescriptor &d = descriptors[i];
    HeatDistributionSimulator sim(d.cols, d.rows, d.cellHeight, d.thermalDiff,
                                   d.sensitivity, d.stepDuration,
                                   localMatrix[i], num_threads);

    sim.run();

    steps[i] = sim.getCurrentStep();
    timeTaken[i] = sim.getPassedTime();

    if (rank != 0) {
      int cells = static_cast<int>(d.rows * d.cols);
      MPI_Send(localMatrix[i], cells, MPI_DOUBLE, 0, kMatrixTag,
                MPI_COMM_WORLD);
      delete[] localMatrix[i];
    }
  }

  // Pull back the final plate for jobs that ran on another rank.
  if (rank == 0) {
    for (int i = 0; i < num_jobs; i++) {
      if (ownerOfJob[i] == 0) continue;
      int cells = static_cast<int>(descriptors[i].rows * descriptors[i].cols);
      MPI_Recv(jobs[i].getTemperatures(), cells, MPI_DOUBLE, ownerOfJob[i],
               kMatrixTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  }

  // steps/timeTaken are zero everywhere except at the owning rank, so a
  // sum-reduce collects them onto rank 0 without any unsafe byte copying.
  std::vector<int> globalSteps(num_jobs, 0);
  std::vector<int> globalTimeTaken(num_jobs, 0);
  MPI_Reduce(steps.data(), globalSteps.data(), num_jobs, MPI_INT, MPI_SUM, 0,
             MPI_COMM_WORLD);
  MPI_Reduce(timeTaken.data(), globalTimeTaken.data(), num_jobs, MPI_INT,
             MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    for (int i = 0; i < num_jobs; i++) {
      jobs[i].setSteps(globalSteps[i]);
      jobs[i].setTimeTaken(globalTimeTaken[i]);
    }

    JobWriter writer(argv[1]);
    writer.setJobs(jobs.data(), num_jobs);
    writer.write();
  }

  MPI_Finalize();  // Finalize the MPI environment
  return 0;
}
