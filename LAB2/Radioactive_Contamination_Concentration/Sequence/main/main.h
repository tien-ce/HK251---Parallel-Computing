#include <math.h>   // For pow
const char *in_path = "../../Input/radioactive_matrix.csv";
const char *out_path = "../../Output/out_radioactive_matrix.csv";
const int rows = 4000;
const int cols = 4000;
const int iterations = 100;
// One point represent 100 meters
const int dx = 100; 
const int dy = 100;
//  Time step : 1 second
const int dt = 1;
// Wind velocity    
const float ux = 3.3;
const float uy = 1.4;
const int D = 1000; // Diffusion coefficient
const float k = 0.01;  //  Deposition rate 10^-2
const float lamda = 0.00003; //  Physical constant representing the decay of radioactive 3 * 10^-5
