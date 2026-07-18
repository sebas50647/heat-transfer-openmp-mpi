// Copyright 2024 mas cquest que nunca
#ifndef PLATE_HPP
#define PLATE_HPP

#include <cstdint>
#include <vector>

/**
 * @brief Forward declaration of Plate class.
 */
template <typename T> class Plate;

/**
 * @brief Represents a cell within a Plate.
 *
 * @tparam T The type of the elements stored in the Plate.
 */
template <typename T> class PlateCell {
 private:
  const Plate<T> *plate;  ///< Pointer to the Plate to which this cell belongs.
  uint64_t column;       ///< The column index of the cell.
  uint64_t row;          ///< The row index of the cell.

 public:
  /**
   * @brief Constructs a PlateCell.
   *
   * @param plate Pointer to the Plate.
   * @param column Column index of the cell.
   * @param row Row index of the cell.
   */
  PlateCell(const Plate<T> *plate, uint64_t &column, uint64_t &row);

  /**
   * @brief Gets the column index of the cell.
   *
   * @return The column index.
   */
  uint64_t getColumn() const;

  /**
   * @brief Gets the row index of the cell.
   *
   * @return The column index.
   */
  uint64_t getRow() const;

  /**
   * @brief Checks if the cell is a border cell.
   *
   * @return True if the cell is a border cell, otherwise false.
   */
  bool isBorderCell() const;

  /**
   * @brief Gets the Plate to which this cell belongs.
   *
   * @return Pointer to the Plate.
   */
  const Plate<T> *getPlate() const;

  /**
   * @brief Gets the adjacent cells to this cell.
   *
   * @return A vector of adjacent PlateCell objects.
   */
  std::vector<PlateCell<T>> getAdjacents() const;
};

/**
 * @brief Represents a matrix of elements.
 *
 * @tparam T The type of the elements stored in the Plate.
 */
template <typename T> class Plate {
 private:
  uint64_t rows;    ///< Number of rows in the Plate.
  uint64_t columns;  ///< Number of columns in the Plate.
  T *matrix;        ///< array of elements.

  uint64_t indexOf(uint64_t column, uint64_t row) const;

 public:
  /**
   * @brief Constructs a Plate.
   *
   * @param rows Number of rows.
   * @param columns Number of columns.
   * @param init 2D array of initial values.
   */
  Plate(const uint64_t &rows, const uint64_t &columns, T *init);

  /**
   * @brief Constructs a Plate.
   * Initializes the constructed plate with an empty 2D matrix
   *
   * @param rows Number of rows.
   * @param columns Number of columns.
   */
  Plate(const uint64_t &rows, const uint64_t &columns);

  /**
   * @brief Copy constructor.
   *
   * @param other The original plate to copy.
   */
  Plate(const Plate<T> &other);

  /**
   * @brief Destructor for Plate.
   */
  ~Plate();

  /**
   * @brief Gets the number of columns in the Plate.
   *
   * @return Number of columns.
   */
  uint64_t getColumns() const;

  /**
   * @brief Gets the number of rows in the Plate.
   *
   * @return Number of rows.
   */
  uint64_t getRows() const;

  /**
   * @brief Gets the value at a specific cell in the Plate.
   *
   * @param column Column index.
   * @param row Row index.
   * @return The value at the specified cell.
   */
  T getValueAt(uint64_t column, uint64_t row) const;

  /**
   * @brief Sets the value at a specific cell in the Plate.
   *
   * @param column Column index.
   * @param row Row index.
   * @param value The value to set.
   */
  void setValueAt(uint64_t column, uint64_t row, T value);

  /**
   * @brief Gets the PlateCell object at a specific cell in the Plate.
   *
   * @param column Column index.
   * @param row Row index.
   * @return The PlateCell object at the specified cell.
   */
  PlateCell<T> getCellAt(uint64_t column, uint64_t row) const;

  /**
   * @brief Get the size of the matrix, thus, the number of cells
   *
   * @return uint64_t the size.
   */
  uint64_t getSize() const;

  /**
   * @brief Gets the row of the cell at the provided index
   *
   * @param i
   * @return uint64_t the row
   */
  uint64_t rowAtIndex(uint64_t i) const;

  /**
   * @brief Gets the column of the cell at the provided index
   *
   * @param i
   * @return uint64_t the column
   */
  uint64_t columnAtIndex(uint64_t i) const;

  /**
   * @brief Checks if a specific cell is a border cell.
   *
   * @param column Column index.
   * @param row Row index.
   * @return True if the cell is a border cell, otherwise false.
   */
  bool isBorderCell(uint64_t column, uint64_t row) const;

  /**
   * @brief Gets the values of the adjacent cells to a specific cell.
   *
   * @param column Column index.
   * @param row Row index.
   * @return A vector of values of the adjacent cells.
   */
  std::vector<T> getAdjacentValuesAt(uint64_t column, uint64_t row) const;

  /**
   * @brief Sets all cells in the Plate to a specific value.
   *
   * @param value The value to set.
   */
  void setAllTo(T value);
};

#include "Plate.tpp"

#endif  // PLATE_HPP
