#pragma once

#include <CSRMatrix.h>
#include <TArray.h>

class Cholesky
{
public:
    CSRMatrix L;

    Cholesky(CSRMatrix &A);
    
    void solve(Vec &b, Vec &x);
private:
    void decomposition();
};