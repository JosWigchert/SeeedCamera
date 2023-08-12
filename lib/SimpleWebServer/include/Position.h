#pragma once

class Position
{
private:
    int row;
    int column;

public:
    Position();
    Position(int row, int column);

    // Comparison operators for Position objects
    bool operator<(const Position &other) const;
    bool operator<=(const Position &other) const;
    bool operator>(const Position &other) const;
    bool operator>=(const Position &other) const;
    bool operator==(const Position &other) const;
    bool operator!=(const Position &other) const;

    int getRow() const;
    int getColumn() const;
    void setRow(int row);
    void setColumn(int column);
};


