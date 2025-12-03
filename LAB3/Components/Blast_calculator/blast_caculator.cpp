#include "blast_caculator.h"
#include <cmath>

// Definition of the Ci coefficients (must occur once in a source file)
const float C_COEFFICIENTS[9] = {
    2.611369,    // C0
    -1.690128,   // C1
    0.00805,     // C2
    0.336743,    // C3
    -0.005162,   // C4
    -0.080923,   // C5
    -0.004785,   // C6
    0.007930,    // C7
    0.000768     // C8
};

/**
 * @breif Calculate the distance from one point to Center
 * Formula: R(i, j) = \sqrt{(i - 1999.5)^2 + (j - 1999.5)^2} x 10 (one point represent 10 met)
 */
float calculate_R(int i, int j) {
    // Center coordinates for a 4000x4000 matrix
    constexpr float CENTER_COORD = 2000;
    constexpr float CELL_SIZE_M = 10.0; // 10 meters per cell

    // 1. Calculate the difference in coordinates
    float di = (float)i - CENTER_COORD;
    float dj = (float)j - CENTER_COORD;

    // 2. Calculate the distance in cell units (Pythagorean theorem)
    float R_cells = std::sqrt(di * di + dj * dj);

    // 3. Convert to actual distance in meters
    return R_cells * CELL_SIZE_M;
}

/**
 * @brief Calculates the Scaled Distance (Z).
 * Formula: Z = R * W^(-1/3)
 */
float calculate_z(float R) {
    // Calculate W^(-1/3) using std::pow for the root operation.
    float W_inv_cbrt = std::pow(YIELD_KG, -1.0 / 3.0);
    return R * W_inv_cbrt;
}

/**
 * @brief Calculates the Intermediate Value (U).
 * Formula: U = -0.21436 + 1.35034 * log10(Z)
 */
float calculate_u(float Z) {
    // Z must be positive. Assuming R > 0 for all cells except the center.
    if (Z <= 0.0) {
        return 0.0; 
    }
    return -0.21436 + 1.35034 * std::log10(Z);
}

/**
 * @brief Calculates the Arrival Time (t).
 * Formula: t = R / SPEED_OF_SOUND
 */
float calculate_t(float R) {
    return R / SPEED_OF_SOUND;
}

/**
 * @brief Calculates Pso and checks the temporal constraint.
 * log10(Pso) = sum(Ci * u^i)
 */
float calculate_pso(float U) {
    // 3. Calculate log10(Pso) using the polynomial summation
    float log10_Pso = 0.0;
    float U_power = 1.0; // Starts at U^0 = 1

    for (int i = 0; i <= 8; ++i) {
        // Accumulate C_i * U^i
        log10_Pso += C_COEFFICIENTS[i] * U_power;
        // Update U_power for the next iteration (U^(i+1))
        U_power *= U; 
    }

    // 4. Calculate Pso: Pso = 10 ^ log10(Pso)
    return std::pow(10.0, log10_Pso);
}