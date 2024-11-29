#include <SKRMatrix.h>
#include <CSRMatrix.h>
#include <iostream>

int main()
{
    SKRMatrix A(3);
    A.elements = {1, 1, 1, 1, 1, 1};
    A.column_offset = {0, 1, 3, 6};

    Vec x = {1, 1, 1};
    Vec y(3);

    A.MVP(x, y);

    std::cout << y << std::endl;

    CSRMatrix M(3);
    M.row_offset = {0, 3, 5, 7};
    M.elm_idx = {0, 1, 2, 1, 2, 1, 2};
    M.elements = {
        4.0, 12.0, -16.0,       // 第一行
             37.0, -43.0,       // 第二行
             -43.0, 98.0        // 第三行
    };

    SKRMatrix B(M);
    B.convertFromCSR(M);
    std::cout << B.elements << std::endl;
    std::cout << B.column_offset << std::endl;
}