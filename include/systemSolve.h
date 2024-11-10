#include <TArray.h>
#include <sparseMatrix.h>
#include <iostream>

/* Since S + M is symmetric and positive definite we can solve
 * the system by the gradient descent method. This is by far
 * not the best method for ill conditionned matrices, but the point
 * here is not (yet) optimality.
 *
 * We need to solve AU = B with A := (S + M).
 * The iterates are constructed as follows : given an approximation
 * U_k we compute the residue
 *
 *          r_k = B - AU_k
 *
 * Then we follow the negative gradient and build
 *
 *                           <r_k, r_k>
 *          U_{k+1} = U_k +  ----------- r_k =: U_k + alpha_k r_k
 *                           <Ar_k, r_k>
 *
 * until the residue becomes smaller to a given threshold.
 *
 * Note : r_{k + 1} = B - A(U_k + alpha_k r_k) = r_k - alpha_k Ar_k
 *        That will save us one matrix vector product.
 */
Vec decentGradientSolve(SparseMatrix &M, SparseMatrix &S, Vec &B, int iterMax = 1000)
{
    int n = M.rows;
    Vec u(n, 0); // Initialize u to (1,1,...,1)
    Vec r(n);    // residue r = B - Au

    double tol = 1e-6;
    int iter = 0;

    Vec Au = SMVP(M, u) + SMVP(S, u);
    r = B - Au;
    std::cout << "r.norm2(): " << r.norm2() << std::endl;

    Vec Ar;
    double alpha;
    while (r.norm() > tol && iter++ < iterMax)
    {
        // calcute alpha
        Ar = SMVP(M, r) + SMVP(S, r);

        double denominator = dot(Ar, r);
        if (denominator == 0)
        {
            throw std::runtime_error("Division by zero in alpha calculation: matrix might be singular or ill-conditioned.");
        }

        alpha = r.norm2() / denominator;

        // update u and r
        u = u + alpha * r;
        r = r - alpha * Ar;

        std::cout << "iter: " << iter << " r.norm2(): " << r.norm2() << std::endl;
    }

    if (iter >= iterMax && r.norm() >= tol)
    {
        std::cerr << "Error: Solution did not converge within the maximum number of iterations." << std::endl;
    }

    return u;
}

/* Since S + M is symmetric and positive definite, we can solve
 * the system using the conjugate gradient method. This method is
 * more efficient than the basic gradient descent, especially for
 * large and ill-conditioned matrices.
 *
 * We need to solve AU = B with A := (S + M).
 * The iterates are constructed as follows: given an approximation
 * U_k, we compute the residue
 *
 *          r_k = B - A U_k
 *
 * We initialize the search direction:
 *
 *          p_0 = r_0
 *
 * Then, for each iteration, we compute the step size alpha_k:
 *
 *                       <r_k, r_k>
 *          alpha_k =  --------------
 *                      <A p_k, p_k>
 *
 * Update the solution:
 *
 *          U_{k+1} = U_k + alpha_k * p_k
 *
 * Update the residue:
 *
 *          r_{k+1} = r_k - alpha_k * A p_k
 *
 * Check for convergence: if ||r_{k+1}|| is less than a given threshold,
 * we consider that the solution has converged.
 *
 * Compute the conjugate direction coefficient beta_k:
 *
 *                   <r_{k+1}, r_{k+1}>
 *          beta_k = ------------------
 *                       <r_k, r_k>
 *
 * Update the search direction:
 *
 *          p_{k+1} = r_{k+1} + beta_k * p_k
 *
 * Note: The conjugate gradient method ensures that each new search
 * direction is A-conjugate to the previous ones, which accelerates
 * convergence compared to the standard gradient descent.
 *
 * Also, since r_{k+1} depends on r_k and A p_k, and p_{k+1} depends
 * on r_{k+1} and p_k, we can optimize computations to avoid extra
 * matrix-vector products.
 */
Vec conjugateGradientSolve(SparseMatrix &M, SparseMatrix &S, Vec &B, int iterMax = 1000)
{
    int n = M.rows;
    Vec u(n, 0);

    Vec Au = SMVP(M, u) + SMVP(S, u); // Au = A @ u
    Vec r0 = B - Au;                  // residue r_k = B - Au_k
    Vec p = r0;                       // initial search direction

    int iter = 0;
    double tol = 1e-6;
    double beta;

    while (iter++ < iterMax)
    {
        Vec Ap = SMVP(M, p) + SMVP(S, p); // A @ p
        double alpha = r0.norm2() / dot(Ap, p);
        u = u + alpha * p;

        Vec r1 = r0 - alpha * Ap; // r_{k+1} = r_k - alpha_k * A @ p_k
        std::cout << "iter: " << iter << " r1.norm(): " << r1.norm() << std::endl;

        if (r1.norm() < tol)
        {
            break;
        }

        // calculate beta
        beta = r1.norm2() / r0.norm2();

        // updata p_k, r_k
        p = r1 + beta * p;
        r0 = r1;
    }

    if (iter >= iterMax && r0.norm() >= tol)
    {
        std::cerr << "Error: Solution did not converge within the maximum number of iterations." << std::endl;
    }

    return u;
}