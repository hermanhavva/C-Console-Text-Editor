//#include "Cursor.h"
#include <iostream>
#include "Cursor class.h"


Cursor::Cursor(int row, int column)
{
    this->row = row;
    this->column = column;
}
void Cursor::SetRow(int row)
{
    this->row = row;
}
void Cursor::SetColumn(int column)
{
    this->column = column;
}
int Cursor::GetRow()
{
    return row;
}
int Cursor::GetColumn()
{
    return column;
}
