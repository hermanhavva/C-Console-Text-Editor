

class Cursor
{
public:
	Cursor(int, int);
	void SetRow(int);
	void SetColumn(int);
	int GetRow();
	int GetColumn();

private:
	int row, column;
};


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
