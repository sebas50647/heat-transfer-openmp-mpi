// Copyright 2024 mas cquest que nunca
#include <cassert>
#include <iostream>

#include "../../src/Plate.hpp"

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

void testIndexingIsRowMajor() {
  // 2 rows x 3 columns, values laid out so each cell equals row*10+column.
  double init[6] = {0, 1, 2, 10, 11, 12};
  Plate<double> plate(2, 3, init);

  check(plate.getValueAt(0, 0) == 0, "getValueAt(0,0) reads row 0 col 0");
  check(plate.getValueAt(1, 0) == 1, "getValueAt(1,0) reads row 0 col 1");
  check(plate.getValueAt(2, 0) == 2, "getValueAt(2,0) reads row 0 col 2");
  check(plate.getValueAt(0, 1) == 10, "getValueAt(0,1) reads row 1 col 0");
  check(plate.getValueAt(2, 1) == 12, "getValueAt(2,1) reads row 1 col 2");
  check(plate.getValueAt(1, 1) != plate.getValueAt(0, 1),
        "different columns in the same row must not alias");
}

void testSetValueAtRoundTrips() {
  Plate<double> plate(2, 3, new double[6]{0, 0, 0, 0, 0, 0});
  plate.setValueAt(2, 0, 42.5);
  plate.setValueAt(0, 1, -3.0);

  check(plate.getValueAt(2, 0) == 42.5, "setValueAt/getValueAt round-trips");
  check(plate.getValueAt(0, 1) == -3.0, "setValueAt does not disturb other rows");
  check(plate.getValueAt(0, 0) == 0, "unset cells stay untouched");
}

void testRowColumnFromIndexAreInverseOfIndexOf() {
  Plate<double> plate(3, 4, new double[12]);
  for (uint64_t row = 0; row < 3; row++) {
    for (uint64_t col = 0; col < 4; col++) {
      uint64_t i = row * 4 + col;
      check(plate.rowAtIndex(i) == row && plate.columnAtIndex(i) == col,
            "rowAtIndex/columnAtIndex invert the linear index");
    }
  }
}

void testBorderCells() {
  Plate<double> plate(3, 3, new double[9]);
  check(plate.isBorderCell(0, 0), "top-left corner is a border cell");
  check(plate.isBorderCell(2, 2), "bottom-right corner is a border cell");
  check(plate.isBorderCell(1, 0), "top edge is a border cell");
  check(!plate.isBorderCell(1, 1), "center cell is not a border cell");
}

void testAdjacentValues() {
  // 3x3 plate, value = row*10 + column.
  double init[9] = {0, 1, 2, 10, 11, 12, 20, 21, 22};
  Plate<double> plate(3, 3, init);

  auto adj = plate.getAdjacentValuesAt(1, 1);
  check(adj.size() == 4, "center cell has 4 neighbors");

  auto cornerAdj = plate.getAdjacentValuesAt(0, 0);
  check(cornerAdj.size() == 2, "corner cell has 2 neighbors");
}

}  // namespace

int main() {
  testIndexingIsRowMajor();
  testSetValueAtRoundTrips();
  testRowColumnFromIndexAreInverseOfIndexOf();
  testBorderCells();
  testAdjacentValues();

  if (failures > 0) {
    std::cerr << failures << " test(s) failed\n";
    return 1;
  }
  std::cout << "All Plate tests passed\n";
  return 0;
}
