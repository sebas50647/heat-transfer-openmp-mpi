// Copyright 2024 mas cquest que nunca
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <iostream>
#include "Plate.hpp"

/**
 * @class HeatDistributionSimulator
 * @brief Simulates heat distribution on a 2D plate.
 */
class HeatDistributionSimulator {
 private:
  Plate<double> *plate;  ///< Pointer to the plate representing the heat grid.
  Plate<double> *scratch;  ///< Buffer holding the state being computed.
  Plate<double> *externalView;  ///< The Plate wrapping the caller's buffer;
                                 ///< never reassigned, used to sync results
                                 ///< back into it once the simulation ends.
  double sensitivity;   ///< Sensitivity threshold for temperature changes.
  double thermalDiff;   ///< Thermal diffusion coefficient.
  int step = 0;         ///< Current simulation step.
  double stepDuration;  ///< Duration of each simulation step.
  double cellHeight;    ///< Height of each cell in the grid.
  int numThreads;       ///< Number of threads to use for parallel computation.

 public:
  /**
   * @brief Constructor with initial temperatures.
   * @param columns Number of columns in the plate.
   * @param rows Number of rows in the plate.
   * @param cellHeight Height of each cell in the grid.
   * @param thermalDiff Thermal diffusion coefficient.
   * @param sensitivity Sensitivity threshold for temperature changes.
   * @param stepDuration Duration of each simulation step.
   * @param initTemps Array of initial temperatures.
   * @param numThreads Number of threads to use for parallel computation.
   */
  HeatDistributionSimulator(const uint64_t &columns, const uint64_t &rows,
                            const double &cellHeight, const double &thermalDiff,
                            const double &sensitivity,
                            const double &stepDuration, double *initTemps,
                            const int &numThreads)
      : sensitivity(sensitivity), thermalDiff(thermalDiff),
        stepDuration(stepDuration), cellHeight(cellHeight),
        numThreads(numThreads) {
    if (initTemps == nullptr) {
      this->plate = new Plate<double>(rows, columns);
      this->plate->setAllTo(0);
    } else {
      this->plate = new Plate<double>(rows, columns, initTemps);
    }
    this->externalView = this->plate;
    this->scratch = new Plate<double>(rows, columns);
    this->scratch->copyValuesFrom(*this->plate);
  }

  /**
   * @brief Constructor without initial temperatures (defaults to zero).
   * @param columns Number of columns in the plate.
   * @param rows Number of rows in the plate.
   * @param cellHeight Height of each cell in the grid.
   * @param thermalDiff Thermal diffusion coefficient.
   * @param sensitivity Sensitivity threshold for temperature changes.
   * @param stepDuration Duration of each simulation step.
   * @param numThreads Number of threads to use for parallel computation.
   */
  HeatDistributionSimulator(const uint64_t &columns, const uint64_t &rows,
                            const double &cellHeight, const double &thermalDiff,
                            const double &sensitivity,
                            const double &stepDuration, const int &numThreads)
      : HeatDistributionSimulator(columns, rows, cellHeight, thermalDiff,
                                  sensitivity, stepDuration, nullptr,
                                  numThreads) {}

  /**
   * @brief Destructor.
   */
  ~HeatDistributionSimulator() {
    delete this->plate;
    delete this->scratch;
  }

  /**
   * @brief Gets the sensitivity threshold.
   * @return Sensitivity threshold.
   */
  double getSensitivity();

  /**
   * @brief Gets the thermal diffusion coefficient.
   * @return Thermal diffusion coefficient.
   */
  double getThermalDiffusion();

  /**
   * @brief Gets the duration of each simulation step.
   * @return Duration of each simulation step.
   */
  double getStepDuration();

  /**
   * @brief Gets the height of each cell in the grid.
   * @return Height of each cell in the grid.
   */
  double getCellHeight();

  /**
   * @brief Gets the temperature at a specific cell.
   * @param column Column index of the cell.
   * @param row Row index of the cell.
   * @return Temperature at the specified cell.
   */
  double getTemperatureAt(int &column, int &row);

  /**
   * @brief Gets the current simulation step.
   * @return Current simulation step.
   */
  int getCurrentStep();

  /**
   * @brief Gets the total passed time in the simulation.
   * @return Total passed time in the simulation.
   */
  int getPassedTime();

  /**
   * @brief Advances the simulation by one step.
   * @return True if the simulation has stabilized, false otherwise.
   */
  bool nextStep();

  /**
   * @brief Runs the simulation until it stabilizes.
   */
  void run();
};

#endif  // SIMULATOR_H
