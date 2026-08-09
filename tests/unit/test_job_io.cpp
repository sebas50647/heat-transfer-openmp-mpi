// Copyright 2024 mas cquest que nunca
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "../../src/Job.hpp"

namespace fs = std::filesystem;

namespace {

int failures = 0;

void check(bool condition, const char *description) {
  if (!condition) {
    std::cerr << "FAIL: " << description << "\n";
    failures++;
  } else {
    std::cout << "PASS: " << description << "\n";
  }
}

void writeBinaryPlate(const fs::path &path, uint64_t rows, uint64_t cols,
                       const std::vector<double> &values) {
  std::ofstream out(path, std::ios::binary);
  out.write(reinterpret_cast<const char *>(&rows), sizeof(rows));
  out.write(reinterpret_cast<const char *>(&cols), sizeof(cols));
  out.write(reinterpret_cast<const char *>(values.data()),
            values.size() * sizeof(double));
}

bool readBinaryPlate(const fs::path &path, uint64_t &rows, uint64_t &cols,
                      std::vector<double> &values) {
  std::ifstream in(path, std::ios::binary);
  if (!in) return false;
  in.read(reinterpret_cast<char *>(&rows), sizeof(rows));
  in.read(reinterpret_cast<char *>(&cols), sizeof(cols));
  values.resize(rows * cols);
  in.read(reinterpret_cast<char *>(values.data()),
          values.size() * sizeof(double));
  return static_cast<bool>(in) || in.eof();
}

// Reader/writer resolve matrix paths relative to the job file's own
// directory when no explicit input/output dir is given -- this is how
// the README's `bin/heatsim job1.txt` invocation behaves.
void testImplicitDirsFromJobFilePath() {
  fs::path workDir = fs::temp_directory_path() / "heatsim_job_io_implicit";
  fs::remove_all(workDir);
  fs::create_directories(workDir);

  std::vector<double> initial = {1, 2, 3, 4, 5, 6};  // 2 rows x 3 cols
  writeBinaryPlate(workDir / "plate.bin", 2, 3, initial);

  std::ofstream jobFile(workDir / "jobs.txt");
  jobFile << "plate.bin 0.5 0.01 1.0 0.0001\n";
  jobFile.close();

  fs::path previousDir = fs::current_path();
  fs::current_path(workDir);

  JobReader reader("jobs.txt");
  reader.read();
  std::vector<Job> &jobs = reader.getJobs();

  check(jobs.size() == 1, "reader parses exactly one job line");
  if (!jobs.empty()) {
    Job &job = jobs[0];
    check(job.getRows() == 2, "job reads the correct row count");
    check(job.getColumns() == 3, "job reads the correct column count");
    check(job.getStepDuration() == 0.5,
          "job reads delta_t in the right column");
    check(job.getThermalDiffusion() == 0.01,
          "job reads alpha in the right column");
    check(job.getCellHeight() == 1.0, "job reads h in the right column");
    check(job.getSensitivity() == 0.0001,
          "job reads epsilon in the right column");

    bool tempsMatch = job.getTemperatures() != nullptr;
    for (int i = 0; tempsMatch && i < 6; i++) {
      if (job.getTemperatures()[i] != initial[i]) tempsMatch = false;
    }
    check(tempsMatch, "job reads the plate's initial temperatures correctly");

    job.setSteps(7);
    job.setTimeTaken(42);

    JobWriter writer("jobs.txt");
    writer.setJobs(jobs.data(), 1);
    writer.write();

    check(fs::exists("tsv/job001.tsv"), "writer produces a per-job tsv report");
    check(fs::exists("tsv/plate001-7.bin"),
          "writer names the result plate with the job id and step count");

    std::ifstream report("tsv/job001.tsv");
    std::string line;
    std::getline(report, line);
    std::istringstream fields(line);
    std::string matrixPath;
    double stepDuration = 0, thermalDiff = 0, cellHeight = 0, sensitivity = 0;
    int steps = 0;
    fields >> matrixPath >> stepDuration >> thermalDiff >> cellHeight >>
        sensitivity >> steps;
    check(matrixPath == "plate.bin" && stepDuration == 0.5 &&
              thermalDiff == 0.01 && cellHeight == 1.0 &&
              sensitivity == 0.0001 && steps == 7,
          "report line preserves the job's parameters and step count");

    uint64_t outRows = 0, outCols = 0;
    std::vector<double> outValues;
    bool readOk =
        readBinaryPlate("tsv/plate001-7.bin", outRows, outCols, outValues);
    check(readOk, "result plate file is readable");
    check(outRows == 2 && outCols == 3,
          "result plate keeps the original dimensions");
    check(outValues == initial, "result plate values round-trip unchanged");
  }

  fs::current_path(previousDir);
  fs::remove_all(workDir);
}

// Reader/writer must also accept an explicit input/output directory,
// independent of where the job description file itself lives.
void testExplicitInputOutputDirs() {
  fs::path base = fs::temp_directory_path() / "heatsim_job_io_explicit";
  fs::remove_all(base);
  fs::path inputDir = base / "plates";
  fs::path outputDir = base / "results";
  fs::create_directories(inputDir);
  fs::create_directories(outputDir);

  std::vector<double> initial = {9, 8, 7, 6};  // 2x2
  writeBinaryPlate(inputDir / "plate.bin", 2, 2, initial);

  fs::path jobFilePath = base / "jobs.txt";
  std::ofstream jobFile(jobFilePath);
  jobFile << "plate.bin 1.0 0.02 0.5 0.001\n";
  jobFile.close();

  JobReader reader(jobFilePath.string(), inputDir);
  reader.read();
  std::vector<Job> &jobs = reader.getJobs();

  check(jobs.size() == 1,
        "reader with an explicit input dir still parses the job");
  if (!jobs.empty()) {
    Job &job = jobs[0];
    bool tempsMatch = job.getTemperatures() != nullptr;
    for (int i = 0; tempsMatch && i < 4; i++) {
      if (job.getTemperatures()[i] != initial[i]) tempsMatch = false;
    }
    check(tempsMatch,
          "reader finds the matrix under the explicit input dir, not "
          "next to the job file");

    job.setSteps(3);
    job.setTimeTaken(10);

    JobWriter writer(jobFilePath.string(), outputDir);
    writer.setJobs(jobs.data(), 1);
    writer.write();

    check(fs::exists(outputDir / "job001.tsv"),
          "writer places the report under the explicit output dir");
    check(fs::exists(outputDir / "plate001-3.bin"),
          "writer places the result plate under the explicit output dir");
  }

  fs::remove_all(base);
}

}  // namespace

int main() {
  testImplicitDirsFromJobFilePath();
  testExplicitInputOutputDirs();

  if (failures > 0) {
    std::cerr << failures << " test(s) failed\n";
    return 1;
  }
  std::cout << "All Job I/O tests passed\n";
  return 0;
}
