#include "Position.h"

Position::Position() : row(0), column(0) {}

Position::Position(int row, int column) : row(row), column(column) {}

// Comparison operators for Position objects
bool Position::operator<(const Position &other) const
{
    if (row < other.row)
        return true;
    if (row == other.row)
        return column < other.column;
    return false;
}

bool Position::operator<=(const Position &other) const
{
    return !(*this > other);
}

bool Position::operator>(const Position &other) const
{
    return other < *this;
}

bool Position::operator>=(const Position &other) const
{
    return !(*this < other);
}

bool Position::operator==(const Position &other) const
{
    return row == other.row && column == other.column;
}

bool Position::operator!=(const Position &other) const
{
    return !(*this == other);
}

int Position::getRow() const
{
    return row;
}

int Position::getColumn() const
{
    return column;
}

void Position::setRow(int row)
{
    this->row = row;
}

void Position::setColumn(int column)
{
    this->column = column;
}