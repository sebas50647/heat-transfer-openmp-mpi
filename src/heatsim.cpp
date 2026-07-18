// Copyright 2024 mas cquest que nunca
#include <omp.h>
#include <mpi.h>
#include <iostream>
#include <vector>

#include "HeatDistributionSimulator.hpp"
#include "Job.hpp"

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

  // Resize the jobs vector on all processes
  jobs.resize(num_jobs);

  // Broadcast the job data to all processes
  Job brodJobs[num_jobs];
  double *matrixes[num_jobs];
  for (int i = 0; i < num_jobs; i++) {
    brodJobs[i] = jobs[i];
    MPI_Bcast(brodJobs + i, sizeof(Job), MPI_BYTE, 0, MPI_COMM_WORLD);

    uint64_t size = brodJobs[i].getColumns() * brodJobs[i].getRows();
    if (rank == 0) {
      matrixes[i] = jobs[i].getTemperatures();
    } else {
      matrixes[i] = new double[size];
    }
    MPI_Bcast(matrixes[i], size * sizeof(double), MPI_BYTE, 0, MPI_COMM_WORLD);
  }
  MPI_Bcast(jobs.data(), num_jobs * sizeof(Job), MPI_BYTE, 0, MPI_COMM_WORLD);

  // Distribute jobs among processes
  int jobs_per_proc = num_jobs / size;
  int remainder = num_jobs % size;
  int start_idx = rank * jobs_per_proc + std::min(rank, remainder);
  int end_idx = start_idx + jobs_per_proc + (rank < remainder);

  std::vector<Job> results;

  for (int i = start_idx; i < end_idx; i++) {
    Job &j = brodJobs[i];

    HeatDistributionSimulator sim(
        j.getColumns(), j.getRows(), j.getCellHeight(), j.getThermalDiffusion(),
        j.getSensitivity(), j.getStepDuration(), matrixes[i], num_threads);

    sim.run();

    j.setSteps(sim.getCurrentStep());
    j.setTimeTaken(sim.getPassedTime());

    results.push_back(j);
  }

  // Gather results from all processes
  Job *all_results = new Job[num_jobs];

  MPI_Gather(results.data(), results.size() * sizeof(Job), MPI_BYTE,
             all_results, results.size() * sizeof(Job), MPI_BYTE, 0,
             MPI_COMM_WORLD);

  if (rank == 0) {
    JobWriter writer(argv[1]);
    writer.setJobs(all_results, num_jobs);
    writer.write();
    // delete all_results;
  }

  MPI_Finalize();  // Finalize the MPI environment
  return 0;
}
