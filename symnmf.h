# ifndef SYMNMF_H_
# define SYMNMF_H_

double** sym_c(double** X, int num_of_coordinates, int N);
double** ddg_c(double** X, int num_of_coordinates, int N);
double** norm_c(double** X, int num_of_coordinates, int N);
double** symnmf_c(double** H, double** W, int N, int k);
double** allocate_matrix(int rows, int cols, int command);
void free_matrix(double** matrix, int num_of_rows);

# endif