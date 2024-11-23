# Repository for a FEM Solver

### To clone the repository

This project requires **GLFW** and **GLM**, which are included as submodules. Clone the repository and its submodules with the following command:

```bash
git clone --recurse-submodules https://github.com/Neroued/femSolver.git
```

### To build the project

Run the following commands to build the project:

```bash
cmake -B build -S .
cmake --build build
```

### Running the Solver

1. To run the Navier-Stokes Solver, use:

   ```bash
   ./build/test_NS {sphere/cube} 100
   ```

   - `{sphere/cube}` specifies the shape of the mesh (either a sphere or a cube).
   - `100` represents the number of subdivisions for the mesh.

2. **Customizing Parameters**

   - To modify the values of  $\Delta t$  (time step) or $\nu$ (kinematic viscosity), go to the end of the `main` function in the source file `/src/test_NS.cpp` and make the necessary changes there.
   - To change the right-hand side (RHS) expression, edit the `test_f` function in `/src/test_NS.cpp`.