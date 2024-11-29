#pragma once

#include <CSRMatrix.h>
#include <SKRMatrix.h>
#include <TArray.h>

class Cholesky
{
public:
    SKRMatrix L;

    Cholesky(CSRMatrix &A);
    
    void solve(Vec &b, Vec &x);
private: 
    void decomposition();
};