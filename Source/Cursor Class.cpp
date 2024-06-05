

class Cursor
{
public:
	Cursor(int row, int column)
	{
		this->row = row;
		this->column = column;
	}
	void SetRow(int row)
	{
		this->row = row;
	}
	void SetColumn(int column)
	{
		this->column = column;
	}
	int GetRow()
	{
		return row;
	}
	int GetColumn()
	{
		return column;
	}
	~Cursor() = default;
private:
	int row, column;
};