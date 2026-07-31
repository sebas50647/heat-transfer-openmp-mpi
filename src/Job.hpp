// Copyright 2024 mas cquest que nunca
#ifndef JOB_HPP
#define JOB_HPP

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

/**
 * @class Job
 * @brief Represents a job to process thermal data.
 */
class Job {
 private:
  std::string matrixPath;  ///< Path to the matrix file.
  double thermalDiff;     ///< Thermal diffusion coefficient.
  double sensitivity;     ///< Sensitivity coefficient.
  uint64_t rows, cols;    ///< Dimensions of the matrix.
  double stepDuration;    ///< Duration of each step.
  double cellHeight;      ///< Height of each cell.
  int timeTaken;          ///< Time taken for the job.
  int steps;              ///< Number of steps taken.
  double *temps;          ///< Temperature values.

 public:
  /**
   * @brief Default constructor.
   */
  Job();

  /**
   * @brief Parameterized constructor.
   * @param rows Number of rows in the matrix.
   * @param cols Number of columns in the matrix.
   * @param temps Pointer to the temperature values.
   * @param thermalDiff Thermal diffusion coefficient.
   * @param sensitivity Sensitivity coefficient.
   * @param stepDuration Duration of each step.
   * @param cellHeight Height of each cell.
   */
  Job(uint64_t rows, uint64_t cols, double *temps, double &thermalDiff,
      double &sensitivity, double &stepDuration, double &cellHeight);

  /**
   * @brief Copy constructor.
   * @param other The Job object to copy from.
   */
  Job(const Job &other);

  /**
   * @brief Assignment operator.
   * @param other The Job object to assign from.
   * @return Reference to the assigned Job object.
   */
  Job &operator=(const Job &other);

  /**
   * @brief Destructor.
   */
  ~Job();

  /**
   * @brief Gets the number of rows in the matrix.
   * @return Number of rows.
   */
  uint64_t getRows() const;

  /**
   * @brief Gets the number of columns in the matrix.
   * @return Number of columns.
   */
  uint64_t getColumns() const;

  /**
   * @brief Gets the path to the matrix file.
   * @return Path to the matrix file.
   */
  std::string getMatrixPath() const;

  /**
   * @brief Gets the temperature values.
   * @return Pointer to the temperature values.
   */
  double *getTemperatures() const;

  /**
   * @brief Gets the thermal diffusion coefficient.
   * @return Thermal diffusion coefficient.
   */
  double getThermalDiffusion() const;

  /**
   * @brief Gets the sensitivity coefficient.
   * @return Sensitivity coefficient.
   */
  double getSensitivity() const;

  /**
   * @brief Gets the duration of each step.
   * @return Duration of each step.
   */
  double getStepDuration() const;

  /**
   * @brief Gets the height of each cell.
   * @return Height of each cell.
   */
  double getCellHeight() const;

  /**
   * @brief Gets the time taken for the job.
   * @return Time taken for the job.
   */
  int getTimeTaken() const;

  /**
   * @brief Gets the number of steps taken.
   * @return Number of steps taken.
   */
  int getSteps() const;

  /**
   * @brief Sets the time taken for the job.
   * @param timeTaken Time taken for the job.
   */
  void setTimeTaken(int timeTaken);

  /**
   * @brief Sets the number of steps taken.
   * @param steps Number of steps taken.
   */
  void setSteps(int steps);

  /**
   * @brief Sets the path to the matrix file.
   * @param path Path to the matrix file.
   */
  void setMatrixPath(std::string path);

  /**
   * @brief Sets the temperature values.
   * @param temps Pointer to the temperature values.
   */
  void setTemperatures(double *temps);
};

/**
 * @class JobReader
 * @brief Reads job data from a file.
 */
class JobReader {
 private:
  std::string filePath;  ///< Path to the file containing job data.
  std::filesystem::path inputDir;  ///< Base directory for input matrices.
  std::vector<Job> jobs;  ///< List of jobs.

  /**
   * @brief Reads a matrix from a file.
   * @param matrixFilePath Path to the matrix file.
   * @param rows Reference to store the number of rows.
   * @param cols Reference to store the number of columns.
   * @return Pointer to the matrix data.
   */
  double *readMatrix(const std::string &matrixFilePath, uint64_t &rows,
                     uint64_t &cols);

 public:
  /**
   * @brief Parameterized constructor.
   * @param path Path to the file containing job data.
   */
  explicit JobReader(std::string path);

  /**
   * @brief Parameterized constructor.
   * @param path Path to the file containing job data.
   * @param inputDir Base directory for matrix files.
   */
  JobReader(std::string path, std::filesystem::path inputDir);

  /**
   * @brief Destructor.
   */
  ~JobReader();

  /**
   * @brief Reads job data from the file.
   */
  void read();

  /**
   * @brief Gets the list of jobs.
   * @return Reference to the list of jobs.
   */
  std::vector<Job> &getJobs();
};

/**
 * @class JobWriter
 * @brief Writes job data to a file.
 */
class JobWriter {
 private:
  std::string filePath;  ///< Path to the file to write job data.
  std::filesystem::path outputDir;  ///< Base directory for output files.
  Job *jobs;            ///< Pointer to the list of jobs.
  int numJobs;          ///< Number of jobs.

  /**
   * @brief Formats time into a string.
   * @param seconds Time in seconds.
   * @return Formatted time string.
   */
  std::string format_time(const time_t seconds);

  /**
   * @brief Writes a matrix to a file.
   * @param filePath Path to the file to write the matrix data.
   * @param matrix Pointer to the matrix data.
   * @param rows Number of rows in the matrix.
   * @param cols Number of columns in the matrix.
   */
  void writeMatrix(const std::string &filePath, double *matrix, uint64_t rows,
                   uint64_t cols);

 public:
  /**
   * @brief Parameterized constructor.
   * @param path Path to the file to write job data.
   */
  explicit JobWriter(std::string path);

  /**
   * @brief Parameterized constructor.
   * @param path Path to the file to write job data.
   * @param outputDir Base directory for output files.
   */
  JobWriter(std::string path, std::filesystem::path outputDir);

  /**
   * @brief Parameterized constructor.
   * @param path Path to the file to write job data.
   * @param jobs List of jobs to write.
   */
  JobWriter(std::string path, std::vector<Job> jobs);

  /**
   * @brief Sets the list of jobs to write.
   * @param jobs Pointer to the list of jobs.
   * @param numJobs Number of jobs.
   */
  void setJobs(Job *jobs, int numJobs);

  /**
   * @brief Writes job data to the file.
   */
  void write();
};

#endif  // JOB_HPP
