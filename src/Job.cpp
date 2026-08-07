// Copyright 2024 mas cquest que nunca
#include "Job.hpp"
#include <omp.h>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <utility>
#include <vector>

Job::Job() {}

Job::Job(uint64_t rows, uint64_t cols, double *temps, double &thermalDiff,
         double &sensitivity, double &stepDuration, double &cellHeight)
    : thermalDiff(thermalDiff), sensitivity(sensitivity), rows(rows),
      cols(cols), stepDuration(stepDuration), cellHeight(cellHeight),
      temps(temps) {}

Job::~Job() {}

Job::Job(const Job &other) {
  this->thermalDiff = other.getThermalDiffusion();
  this->sensitivity = other.getSensitivity();
  this->rows = other.getRows();
  this->cols = other.getColumns();
  this->stepDuration = other.getStepDuration();
  this->cellHeight = other.getCellHeight();
  this->temps = other.getTemperatures();
  this->steps = other.getSteps();
  this->timeTaken = other.getTimeTaken();
  this->matrixPath = other.getMatrixPath();
}

Job &Job::operator=(const Job &other) {
  this->thermalDiff = other.getThermalDiffusion();
  this->sensitivity = other.getSensitivity();
  this->rows = other.getRows();
  this->cols = other.getColumns();
  this->stepDuration = other.getStepDuration();
  this->cellHeight = other.getCellHeight();
  this->temps = other.getTemperatures();
  this->steps = other.getSteps();
  this->timeTaken = other.getTimeTaken();
  this->matrixPath = other.getMatrixPath();

  return *this;
}

uint64_t Job::getRows() const { return this->rows; }

uint64_t Job::getColumns() const { return this->cols; }

std::string Job::getMatrixPath() const { return this->matrixPath; }

double *Job::getTemperatures() const { return this->temps; }

double Job::getThermalDiffusion() const { return this->thermalDiff; }

double Job::getSensitivity() const { return this->sensitivity; }

double Job::getStepDuration() const { return this->stepDuration; }

double Job::getCellHeight() const { return this->cellHeight; }

int Job::getTimeTaken() const { return this->timeTaken; }

int Job::getSteps() const { return this->steps; }

void Job::setTimeTaken(int timeTaken) { this->timeTaken = timeTaken; }

void Job::setSteps(int steps) { this->steps = steps; }

void Job::setMatrixPath(std::string path) { this->matrixPath = path; }

void Job::setTemperatures(double *temps) { this->temps = temps; }

JobReader::~JobReader() {}

JobReader::JobReader(std::string path, std::filesystem::path inputDir)
  : filePath(path), inputDir(std::move(inputDir)) {}

double *JobReader::readMatrix(const std::string &matrixFilePath, uint64_t &rows,
                              uint64_t &cols) {
  std::ifstream infile(matrixFilePath, std::ios::binary);
  if (!infile) {
    std::cerr << "Cannot open file: " << matrixFilePath << std::endl;
    return nullptr;
  }

  // Read the number of rows and columns
  infile.read(reinterpret_cast<char *>(&rows), sizeof(rows));
  infile.read(reinterpret_cast<char *>(&cols), sizeof(cols));

  double *matrix = new double[cols * rows];
  infile.read(reinterpret_cast<char *>(matrix), rows * cols * sizeof(double));

  infile.close();
  return matrix;
}

JobReader::JobReader(std::string path) : filePath(path) {}

void JobReader::read() {
  std::ifstream infile(filePath);
  if (!infile) {
    std::cerr << "Cannot open file: " << filePath << std::endl;
    return;
  }
  std::string line;
  while (std::getline(infile, line)) {
    std::istringstream iss(line);
    std::string matrixFilePath;
    double stepDuration, cellHeight;
    double thermalDiff, sensitivity;
    uint64_t rows, cols;
    double *temps;

    if (!(iss >> matrixFilePath >> stepDuration >> thermalDiff >> cellHeight >>
          sensitivity)) {
      std::cerr << "Error reading line: " << line << std::endl;
      continue;
    }

    std::filesystem::path relativeMatrixPath = this->inputDir / matrixFilePath;
    if (this->inputDir.empty()) {
      std::filesystem::path path = this->filePath;
      relativeMatrixPath = path.parent_path() / matrixFilePath;
    }

    temps = readMatrix(relativeMatrixPath.string(), rows, cols);
    if (!temps) {
      continue;
    }

    Job job(rows, cols, temps, thermalDiff, sensitivity, stepDuration,
            cellHeight);
    job.setMatrixPath(matrixFilePath);
    jobs.push_back(job);
  }

  infile.close();
}

std::vector<Job> &JobReader::getJobs() { return jobs; }

JobWriter::JobWriter(std::string path) : filePath(path), jobs() {}

JobWriter::JobWriter(std::string path, std::filesystem::path outputDir)
  : filePath(path), outputDir(std::move(outputDir)), jobs() {}

void JobWriter::setJobs(Job *jobs, int numJobs) {
  this->jobs = jobs;
  this->numJobs = numJobs;
}

std::string JobWriter::format_time(const time_t seconds) {
  char text[48];  // YYYY/MM/DD hh:mm:ss
  time_t days = seconds / 86400;
  time_t remaining = seconds % 86400;
  if (remaining < 0) {
    remaining += 86400;
    --days;
  }

  const int year = static_cast<int>(days / 360) + 1970;
  days %= 360;
  const int month = static_cast<int>(days / 30) + 1;
  const int day = static_cast<int>(days % 30) + 1;
  const int hour = static_cast<int>(remaining / 3600);
  remaining %= 3600;
  const int minute = static_cast<int>(remaining / 60);
  const int second = static_cast<int>(remaining % 60);

  snprintf(text, sizeof text, "%04d/%02d/%02d\t%02d:%02d:%02d", year,
           month, day, hour, minute, second);
  return text;
}

void JobWriter::writeMatrix(const std::string &filePath, double *matrix,
                            uint64_t rows, uint64_t cols) {
  std::ofstream outFile(filePath, std::ios::binary);
  if (!outFile) {
    std::cerr << "Cannot open file: " << filePath << std::endl;
    return;
  }

  outFile.write(reinterpret_cast<const char *>(&rows), sizeof(rows));
  outFile.write(reinterpret_cast<const char *>(&cols), sizeof(cols));
  outFile.write(reinterpret_cast<const char *>(matrix),
                cols * rows * sizeof(double));

  outFile.close();
}

void JobWriter::write() {
  std::filesystem::path dirPath = this->outputDir.empty()
                                      ? std::filesystem::path("tsv")
                                      : this->outputDir;
  if (!std::filesystem::exists(dirPath)) {
    std::filesystem::create_directories(dirPath);
  }

  for (int i = 0; i < this->numJobs; ++i) {
    const int jobNumber = i + 1;
    std::ostringstream jobId;
    jobId << std::setw(3) << std::setfill('0') << jobNumber;

    Job &job = this->jobs[i];

    std::filesystem::path jobFileName = dirPath / ("job" + jobId.str() + ".tsv");
    std::filesystem::path matrixFileName =
        dirPath / ("plate" + jobId.str() + "-" + std::to_string(job.getSteps()) +
                   ".bin");

    std::ostringstream oss;
    oss << job.getMatrixPath() << "\t" << job.getStepDuration() << "\t"
        << job.getThermalDiffusion() << "\t" << job.getCellHeight() << "\t"
        << job.getSensitivity() << "\t" << job.getSteps() << "\t"
        << format_time(job.getSteps() * job.getStepDuration()) << "\n";

    std::ofstream outFile(jobFileName);
    if (!outFile) {
      std::cerr << "Cannot open file: " << jobFileName.string() << std::endl;
      continue;
    }

    outFile << oss.str();
    outFile.close();

    writeMatrix(matrixFileName.string(), job.getTemperatures(), job.getRows(),
                job.getColumns());
  }
}
