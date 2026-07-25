// Copyright 2024 mas cquest que nunca
#include "HeatDistributionSimulator.hpp"
#include <omp.h>
#include <cmath>

double HeatDistributionSimulator::getSensitivity() { return this->sensitivity; }

double HeatDistributionSimulator::getThermalDiffusion() {
  return this->thermalDiff;
}

double HeatDistributionSimulator::getStepDuration() {
  return this->stepDuration;
}

double HeatDistributionSimulator::getCellHeight() { return this->cellHeight; }

double HeatDistributionSimulator::getTemperatureAt(int &column, int &row) {
  return this->plate->getValueAt(column, row);
}

int HeatDistributionSimulator::getCurrentStep() { return this->step; }

int HeatDistributionSimulator::getPassedTime() {
  return this->step * this->stepDuration;
}

bool HeatDistributionSimulator::nextStep() {
  Plate<double> prev = *this->plate;
  int cells = this->plate->getColumns() * this->plate->getRows();
  bool localResult = true;
  this->step++;
#pragma omp parallel for reduction(&& : localResult) num_threads(this->numThreads)
  for (int i = 0; i < cells; i++) {
    int c = prev.columnAtIndex(i);
    int r = prev.rowAtIndex(i);
    double val = prev.getValueAt(c, r);

    if (prev.isBorderCell(c, r)) {
      continue;
    }

    double distributed = -(4 * val);
    for (double t : prev.getAdjacentValuesAt(c, r)) {
      distributed += t;
    }

    distributed = distributed * this->stepDuration * this->thermalDiff;
    distributed = distributed / pow(this->cellHeight, 2);
    distributed += val;

    this->plate->setValueAt(c, r, distributed);

    if (abs(val - distributed) > this->sensitivity) {
      localResult = false;
    }
  }

  return localResult;
}

void HeatDistributionSimulator::run() {
  while (!this->nextStep())
    ;
}
