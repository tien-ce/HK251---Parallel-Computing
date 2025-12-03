#define NUM_THREAD 4
const char *in_path = "../../Input/blast_matrix.csv";
const char *out_path = "../../Output/parl_blast_matrix.csv";
const int rows = 4000;
const int cols = 4000;
const int iterations = 100;
struct Blast_parameter {
    int start_row;
    int end_row;
    int cols; 
    float current_sim_time;
    float* output_matrix; 
    Blast_parameter (int start_row,
    int end_row,
    int cols, 
    float current_sim_time,
    float* output_matrix 
    ) : start_row (start_row), end_row (end_row), cols (cols), current_sim_time (current_sim_time), output_matrix (output_matrix){} 
};