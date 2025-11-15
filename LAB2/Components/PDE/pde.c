#include "pde.h"
// --- Term 1: Advection (Wind Effect) ---
float calculate_advection(float C_ij, float C_i_jm1, float C_im1_j, 
                           const float ux, const float uy, const float dx, const float dy) {
    // Formula: Advection Flux = ux * (C[i][j] - C[i][j-1]) / dx + uy * (C[i][j] - C[i-1][j]) / dy
    // Term for dC/dt is: - (Advection Flux)

    // X-component flux (East-West flow)
    float term1_x_flux = ux * (C_ij - C_i_jm1) / dx; 
    
    // Y-component flux (North-South flow)
    float term1_y_flux = uy * (C_ij - C_im1_j) / dy;
    
    // The Advection contribution to dC/dt 
    return -(term1_x_flux + term1_y_flux);
}

// --- Term 2: Diffusion (Spreading Effect) ---
float calculate_diffusion(float C_ij, float C_ip1_j, float C_im1_j, float C_i_jp1, float C_i_jm1, 
                           const float D, const float dx, const float dy) {
    // Discrete Laplacian: 
    // differentiation^2 C = (C[i][j+1] - 2*C[i][j] + C[i][j-1]) / dx^2 
    //           + (C[i+1][j] - 2*C[i][j] + C[i-1][j]) / dy^2
    
    // X-component of the Discrete Laplacian (East and West neighbors)
    float term2_x = (C_i_jp1 - 2.0 * C_ij + C_i_jm1) / (dx * dx);
    
    // Y-component of the Discrete Laplacian (North and South neighbors)
    float term2_y = (C_ip1_j - 2.0 * C_ij + C_im1_j) / (dy * dy);
    
    // Total Diffusion contribution to dC/dt
    return D * (term2_x + term2_y);
}

// --- Term 3: Decay (Fading Effect) ---
float calculate_decay(const float C_ij, const float lambda, const float k) {
    // Formula: Decay Term = - (lambda + k) * C[i][j]
    
    float DecayRate = lambda + k;
    
    return - (DecayRate * C_ij);
}

// --- Final Update Function ---
float update_concentration(float C_old, float dC_dt, const float dt) {
    // Final iterative formula: C_new = C_old + dt * (dC/dt)
    
    float C_new = C_old + dt * dC_dt;

    // Concentrations must not become negative.
    return C_new > 0.00 ? C_new : 0.00;
}