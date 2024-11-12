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
bool decentGradientSolve(SparseMatrix &M, SparseMatrix &S, Vec &B, Vec &u, double tol, int iterMax = 1000)
{
    int n = M.rows;
    Vec r(n); // residue r = B - Au

    int iter = 0;

    // Au = M * u + S * u
    Vec Au = MVP(M, u);
    Au.addInPlace(MVP(S, u));

    // r = B - Au
    blas_axpby(1.0, B, -1.0, Au, r);

    Vec Ar(n);
    Vec Mr(n);
    Vec Sr(n);
    double alpha;
    double r_norm = r.norm();
    while (r_norm > tol && iter++ < iterMax)
    {
        // calcute alpha

        // Ar = M * r + S * r
        M.MVP(r, Mr);
        S.MVP(r, Sr);
        blas_axpby(1.0, Mr, 1.0, Sr, Ar);

        double denominator = dot(Ar, r);
        if (denominator == 0)
        {
            throw std::runtime_error("Division by zero in alpha calculation: matrix might be singular or ill-conditioned.");
        }

        // alpha = (r^T * r) / (r^T * A * r)
        alpha = r_norm * r_norm / denominator;

        // update u and r
        // u = u + alpha * r
        blas_axpby(1.0, u, alpha, r, u);
        // r = r - alpha * A * r
        blas_axpby(1.0, r, -alpha, Ar, r);
        r_norm = r.norm();
    }

    if (iter >= iterMax && r.norm() >= tol)
    {
        std::cerr << "Error: Solution did not converge within the maximum number of iterations." << std::endl;
        return false;
    }
    else
    {
        std::cout << "decentGraident: Converge in " << iter << " iters" << std::endl;
        return true;
    }
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
bool conjugateGradientSolve(SparseMatrix &M, SparseMatrix &S, Vec &B, Vec &u, double tol, int iterMax = 1000)
{
    using namespace std;
    int n = M.rows;

    // Au = M * u + S * u
    Vec Au = MVP(M, u);
    Au.addInPlace(MVP(S, u));
    Vec r0 = B - Au; // residue r_k = B - Au_k
    Vec p = r0;      // initial search direction

    int iter = 0;
    double beta;

    Vec Ap(n);
    Vec Mp(n);
    Vec Sp(n);
    Vec r1(n);
    double r0_norm2;
    double r1_norm2;
    while (iter++ < iterMax)
    {   
        M.MVP(p, Mp);
        S.MVP(p, Sp);
        blas_axpby(1.0, Mp, 1.0, Sp, Ap);

        r0_norm2 = r0.norm2();
        double alpha = r0_norm2 / dot(p, Ap);
        blas_axpby(1.0, u, alpha, p, u);

        blas_axpby(1.0, r0, -alpha, Ap, r1);
        //Vec r1 = r0 - alpha * Ap; // r_{k+1} = r_k - alpha_k * A @ p_k
        r1_norm2 = r1.norm2();

        if (r1_norm2 < tol * tol)
        {
            break;
        }

        // calculate beta
        beta = r1_norm2 / r0_norm2;

        // updata p_k, r_k
        blas_axpby(1.0, r1, beta, p, p);
        // p = r1 + beta * p;
        r0 = r1;
    }

    if (iter >= iterMax && r0.norm() >= tol)
    {
        std::cerr << "Error: Solution did not converge within the maximum number of iterations." << std::endl;
        return false;
    }
    else
    {
        std::cout << "conjugateGradient: Converge in " << iter << " iters" << std::endl;
        return true;
    }
}