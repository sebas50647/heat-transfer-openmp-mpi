// Copyright 2024 mas cquest que nunca
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {

bool readPlate(const std::string &path, uint64_t &rows, uint64_t &cols,
               std::vector<double> &values) {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    std::cerr << "Cannot open file: " << path << std::endl;
    return false;
  }

  file.read(reinterpret_cast<char *>(&rows), sizeof(rows));
  file.read(reinterpret_cast<char *>(&cols), sizeof(cols));
  values.resize(rows * cols);
  file.read(reinterpret_cast<char *>(values.data()),
            values.size() * sizeof(double));

  if (!file) {
    std::cerr << "Truncated or malformed plate file: " << path << std::endl;
    return false;
  }
  return true;
}

}  // namespace

int main(int argc, char *argv[]) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <file1> <file2> <epsilon>\n";
    return 1;
  }

  const double epsilon = std::strtod(argv[3], nullptr);

  uint64_t rows1 = 0, cols1 = 0, rows2 = 0, cols2 = 0;
  std::vector<double> values1, values2;
  if (!readPlate(argv[1], rows1, cols1, values1) ||
      !readPlate(argv[2], rows2, cols2, values2)) {
    return 1;
  }

  if (rows1 != rows2 || cols1 != cols2) {
    std::cerr << "Dimension mismatch: " << rows1 << "x" << cols1 << " vs "
               << rows2 << "x" << cols2 << std::endl;
    return 1;
  }

  uint64_t mismatches = 0;
  const uint64_t kMaxReported = 10;
  for (uint64_t i = 0; i < values1.size(); ++i) {
    if (std::fabs(values1[i] - values2[i]) > epsilon) {
      if (mismatches < kMaxReported) {
        std::cerr << "Mismatch at (" << i / cols1 << ", " << i % cols1
                   << "): " << values1[i] << " vs " << values2[i]
                   << std::endl;
      }
      ++mismatches;
    }
  }

  if (mismatches > 0) {
    std::cerr << mismatches << " of " << values1.size()
               << " cells differ by more than " << epsilon << std::endl;
    return 1;
  }

  std::cout << "OK: plates match within epsilon " << epsilon << std::endl;
  return 0;
}
