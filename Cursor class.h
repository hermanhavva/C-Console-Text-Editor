
#ifndef _CLASS_CURSOR_
#define _CLASS_CURSOR_

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

#endif // !_CLASS_CURSOR_
