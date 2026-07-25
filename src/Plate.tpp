// Copyright 2024 mas cquest que nunca
#include "Plate.hpp"

// PlateCell implementation
template <typename T>
PlateCell<T>::PlateCell(const Plate<T> *plate, uint64_t &column, uint64_t &row)
    : plate(plate), column(column), row(row) {}

template <typename T>
uint64_t PlateCell<T>::getColumn() const {
    return this->column;
}

template <typename T>
uint64_t PlateCell<T>::getRow() const {
    return this->row;
}

template <typename T>
bool PlateCell<T>::isBorderCell() const {
    return this->plate->isBorderCell(column, row);
}

template <typename T>
const Plate<T> *PlateCell<T>::getPlate() const {
    return this->plate;
}

template <typename T>
std::vector<PlateCell<T>> PlateCell<T>::getAdjacents() const {
    std::vector<PlateCell<T>> adjacents;
    // Example logic, assuming adjacency means 4-connected cells
    if (column > 0)
        adjacents.push_back(this->plate->getCellAt(column - 1, row));
    if (column < plate->getColumns() - 1)
        adjacents.push_back(this->plate->getCellAt(column + 1, row));
    if (row > 0)
        adjacents.push_back(this->plate->getCellAt(column, row - 1));
    if (row < plate->getRows() - 1)
        adjacents.push_back(this->plate->getCellAt(column, row + 1));
    return adjacents;
}

// Plate implementation
template <typename T>
Plate<T>::Plate(const uint64_t &rows, const uint64_t &columns, T *init) : rows(rows), columns(columns), matrix(init) {}
    
template <typename T>
Plate<T>::Plate(const uint64_t &rows, const uint64_t &columns) : Plate(rows, columns, new T[rows*columns]) {}

template <typename T>
Plate<T>::Plate(const Plate<T> &other) : Plate(other.getRows(), other.getColumns(), new T[other.getSize()]) {
    for (uint64_t i = 0; i < other.getSize(); i++) {
        this->matrix[i] = other.matrix[i];
    }
}

template <typename T>
Plate<T>::~Plate() {
    /*
    for(uint64_t i = 0; i < columns; i++) {
        delete[] this->matrix[i];
    }
    delete[] this->matrix;*/
}

template <typename T>
uint64_t Plate<T>::getColumns() const {
    return this->columns;
}

template <typename T>
uint64_t Plate<T>::getRows() const {
    return this->rows;
}

template <typename T>
T Plate<T>::getValueAt(uint64_t column, uint64_t row) const {
    int i = this->indexOf(column, row);
    return this->matrix[i];
}

template <typename T>
void Plate<T>::setValueAt(uint64_t column, uint64_t row, T value) {
    this->matrix[indexOf(column, row)] = value;
}

template <typename T>
PlateCell<T> Plate<T>::getCellAt(uint64_t column, uint64_t row) const {
    return PlateCell<T>(this, column, row);
}

template <typename T>
bool Plate<T>::isBorderCell(uint64_t column, uint64_t row) const {
    return (column == 0 || column == columns - 1 || row == 0 || row == rows - 1);
}

template <typename T>
std::vector<T> Plate<T>::getAdjacentValuesAt(uint64_t column, uint64_t row) const {
    std::vector<T> adjacents;
    if (column > 0)
        adjacents.push_back(getValueAt(column - 1, row));
    if (column < columns - 1)
        adjacents.push_back(getValueAt(column + 1, row));
    if (row > 0)
        adjacents.push_back(getValueAt(column, row - 1));
    if (row < rows - 1)
        adjacents.push_back(getValueAt(column, row + 1));
    return adjacents;
}

template <typename T>
uint64_t Plate<T>::indexOf(uint64_t column, uint64_t row) const {
    return row * this->getColumns() + column;
}

template <typename T>
uint64_t Plate<T>::getSize() const {
    return this->getColumns() * this->getRows();
}

template <typename T>
uint64_t Plate<T>::rowAtIndex(uint64_t i) const {
    return i / this->getColumns();
}

template <typename T>
uint64_t Plate<T>::columnAtIndex(uint64_t i) const {
    return i % this->getColumns();
}

template <typename T>
void Plate<T>::setAllTo(T value) {
    for (uint64_t i = 0; i < this->getSize(); ++i) {
        this->matrix[i] = value;
    }
}