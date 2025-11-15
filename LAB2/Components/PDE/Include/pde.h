#ifndef PDE_H
#define PDE_H
/**
 * @brief Calculates the Advection term (Term 1: -u*differentiation C) using the Upwind Scheme.
 * @param C_ij Current cell concentration.
 * @param C_i_jm1 Upwind concentration in the x-direction (C[i][j-1]).
 * @param C_im1_j Upwind concentration in the y-direction (C[i-1][j]).
 * @param ux Wind velocity x-component (m/s).
 * @param uy Wind velocity y-component (m/s).
 * @param dx Spatial step size dx (m).
 * @param dy Spatial step size dy (m).
 * @return The Advection term contribution to dC/dt.
 */
float calculate_advection(float C_ij, float C_i_jm1, float C_im1_j, 
                           const float ux, const float uy, const float dx, const float dy);

/**
 * @brief Calculates the Diffusion term (Term 2: D*differentiation^2 C) using the Discrete Laplacian.
 * @param C_ij Current cell concentration.
 * @param C_ip1_j South neighbor concentration (C[i+1][j]).
 * @param C_im1_j North neighbor concentration (C[i-1][j]).
 * @param C_i_jp1 East neighbor concentration (C[i][j+1]).
 * @param C_i_jm1 West neighbor concentration (C[i][j-1]).
 * @param D Diffusion coefficient.
 * @param dx Spatial step size dx (m).
 * @param dy Spatial step size dy (m).
 * @return The Diffusion term contribution to dC/dt.
 */
float calculate_diffusion(float C_ij, float C_ip1_j, float C_im1_j, float C_i_jp1, float C_i_jm1, 
                           const float D, const float dx, const float dy);

/**
 * @brief Calculates the Decay/Deposition term (Term 3: -(lambda+k)*C).
 * @param C_ij Current cell concentration.
 * @param lambda Radioactive decay constant.
 * @param k Deposition rate.
 * @return The Decay term contribution to dC/dt.
 */
float calculate_decay(const float C_ij, const float lambda, const float k);

/**
 * @brief Calculates the new concentration C_new based on the old value and dC/dt.
 * @param C_old Current cell concentration at t_old.
 * @param dC_dt Total rate of change (Advection + Diffusion + Decay).
 * @param dt Time step size.
 * @return The new cell concentration C_new at t_new (guaranteed non-negative).
 */
float update_concentration(float C_old, float dC_dt, const float dt);
#endif //PDE_H