#ifndef BLAST_CALCULATOR_H
#define BLAST_CALCULATOR_H

#include <cmath>
#include <iostream>

// Constant for the explosive yield conversion (5000 kilotons to kilograms).
// W must be in kilograms for the Z formula: 1 kt TNT is approximately 10^6 kg.
constexpr float YIELD_KT = 5000.0;
constexpr float YIELD_KG = YIELD_KT * 1e6; 
constexpr float SPEED_OF_SOUND = 343.0; // Speed of sound (m/s) for time approximation

// Ci coefficients for the log10(Pso) polynomial.
// Declared here, defined in the .cpp file to comply with the One Definition Rule (ODR).
extern const float C_COEFFICIENTS[9];

/**
 * @brief Calculates the actual distance R (in meters) from the center to a cell (i, j).
 * @param i Row index of the cell.
 * @param j Column index of the cell.
 * @return The actual distance R (meters).
 */
float calculate_R(int i, int j);

/**
 * @brief Calculates the Scaled Distance (Z).
 * Formula: Z = R * W^(-1/3)
 * @param R Actual distance from the blast center (meters).
 * @return The Scaled Distance Z (m/kg^(1/3)).
 */
float calculate_z(float R);

/**
 * @brief Calculates the Intermediate Value (U) for the Pso polynomial.
 * Formula: U = -0.21436 + 1.35034 * log10(Z)
 * @param Z Scaled Distance.
 * @return The intermediate value U (dimensionless).
 */
float calculate_u(float Z);

/**
 * @brief Calculates the Arrival Time (t) of the shock wave.
 * Formula: t = R / 343
 * @param R Actual distance from the blast center (meters).
 * @return The arrival time t (seconds).
 */
float calculate_t(float R);

/**
 * @brief Calculates the Peak Overpressure (Pso) and checks the temporal constraint.
 * @param U The intermediate value calculated by calculate_u.
 * @param current_simulation_time The current time step of the simulation (seconds).
 * @return The Peak Overpressure Pso (kPa) if the shock wave has arrived, or 0.0 otherwise.
 */
float calculate_pso(float U);
#endif