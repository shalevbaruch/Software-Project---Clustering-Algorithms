#define _GNU_SOURCE
#include<stdio.h>
#include <stdlib.h>
#include <math.h>
#include<string.h>
#include "symnmf.h"


int initialize(double *** arr_ptr, int * num_of_coordinates_ptr, int * N_ptr, char* filepath);
void enter_datapoint(double *arr, char* line);
int chr_until_comma(const char * str);
double euclidean_distance(double *datapoint1, double *datapoint2, int num_of_coordinates);
double** sym_c(double** X, int num_of_coordinates, int N);
double** ddg_c(double** X, int num_of_coordinates, int N);
void free_matrix(double** matrix, int num_of_rows);
double** norm_c(double** X, int num_of_coordinates, int N);
void print_matrix(double** matrix, int num_of_rows, int num_of_columns);
double** transpose_matrix(double** matrix, int rows, int cols);
double** update_H(double **H, double** W, int rows, int cols);
double** subtract_matrix(double** mat1, int rows1, int cols1, double** mat2, int rows2, int cols2);
double** allocate_matrix(int rows, int cols, int command);
double** symnmf_c(double** H, double** W, int N, int k);
double** matrix_multiply(double** mat1, int ROWS1, int COLS1, double** mat2, int ROWS2, int COLS2);


int main(int argc, char* argv[]){
   char* goal = argv[1];
   char* filepath = argv[2];
   int num_of_coordinates;
   int N;
   double** X = NULL; /* X is the datapoints array*/
   double** result; /* result is the output*/
   (void)argc;
   if (initialize(&X, &num_of_coordinates, &N, filepath) != 0){ 
        printf("An Error Has Occurred");
        return 1;
   }
   
   if (!strcmp(goal, "sym")){
      if ((result = sym_c(X,num_of_coordinates,N)) == NULL){
         printf("An Error Has Occurred");
         return 1;
      }
   }
   else if (!strcmp(goal, "ddg")){
      if ((result = ddg_c(X,num_of_coordinates,N)) == NULL){
         printf("An Error Has Occurred");
         return 1;
      }
   }
   else{
      if ((result = norm_c(X,num_of_coordinates,N)) == NULL){
         printf("An Error Has Occurred");
         return 1;
      }
   }    
   print_matrix(result, N, N);
   free_matrix(X, N);
   free_matrix(result, N);
   return 0;
}


int initialize(double *** arr_ptr, int * num_of_coordinates_ptr, int * N_ptr, char* filepath) {
   char *line = NULL;
   size_t len = 0;
   size_t nread;
   size_t i;
   int num_of_coordinates = 1;
   int capacity = 0;
   int num_of_datapoints = 0;
   double** arr = NULL;
   FILE *file = fopen(filepath, "r");
   if (file == NULL) {
      return 1;
   }
   while ((nread = getline(&line, &len, file)) != (size_t)-1){
      if (num_of_datapoints == 0){
      for (i = 0; i < nread; i++) {
         if (line[i] == ',') { /* If we find a comma, increment the count */
               num_of_coordinates++;
         }
      }
      }
      if (num_of_datapoints == capacity){ /*If all of the memory of the current array is full, we need to relocate it*/
         if (capacity == 0){
               capacity = 128;
         }
         capacity = capacity * 2;
         arr = (double**)realloc(arr, capacity * sizeof(double*));
         if (arr == NULL) {
               return 1;
         }
      }
      arr[num_of_datapoints] = (double*)malloc(num_of_coordinates * sizeof(double));
      if (arr[num_of_datapoints] == NULL) {
         return 1;
      }
      enter_datapoint(arr[num_of_datapoints], line);
      num_of_datapoints++;
   }
   free(line);
   *N_ptr = num_of_datapoints;
   *num_of_coordinates_ptr = num_of_coordinates;
   *arr_ptr = arr;
   fclose(file);
   return 0; 
}


void enter_datapoint(double *arr, char* line){ /* Copy the datapoint into the array of datapoints*/
    int i = 0;
    int pos = 0;
    while (sscanf(line + pos, "%lf", &arr[i]) == 1) {
        pos += chr_until_comma(line + pos) + 1;
        if ((line[pos-1]) == '\0'){
            break;
        }
        i++;
    }
}


int chr_until_comma(const char * str){
    int i = 0;
    if (str == NULL){
        return 0;
    }
    while (str[i] != '\0' && str[i] != ','){
        i++;
    }
    return i;
}


double** sym_c(double** X, int num_of_coordinates, int N){ /* Computing the similarity matrix */
   int i;
   int j;
   double sim_value;
   double** similarity_matrix = allocate_matrix(N, N, 1);
   double euc_distance;
   if (similarity_matrix == NULL){
      return NULL;
   }

   for (i = 0; i < N; i++) {
      for (j = 0; j < i; j++) {
         euc_distance = euclidean_distance(X[i], X[j], num_of_coordinates);
         sim_value = exp(-euc_distance * euc_distance / 2);
         similarity_matrix[i][j] = sim_value;
         similarity_matrix[j][i] = sim_value; /* Copy to the lower triangular part (symmetric property) */
      }

      similarity_matrix[i][i] = 0;
   }
   return similarity_matrix;
}


double** ddg_c(double** X, int num_of_coordinates, int N){ /* Computing the diagonal matrix*/
   int i;
   int j;
   double row_sum;
   double** diagonal_matrix = allocate_matrix(N, N, 0);
   double** similarity_matrix = sym_c(X, num_of_coordinates, N);
   if (similarity_matrix == NULL || diagonal_matrix == NULL){
      return NULL;
   }
   for(i=0; i<N; i++){
      row_sum = 0;
      for(j=0; j<N; j++){
         row_sum += similarity_matrix[i][j];
      }
      diagonal_matrix[i][i] = row_sum;
   }
   free_matrix(similarity_matrix, N);
   return diagonal_matrix;
}


double** norm_c(double** X, int num_of_coordinates, int N){  /* Computing the normalized similarity matrix */
   double** similarity_matrix = sym_c(X, num_of_coordinates, N);
   double** diagonal_matrix = ddg_c(X, num_of_coordinates, N);
   double **W, **tmp_W;
   int i; 
   if (similarity_matrix == NULL || diagonal_matrix == NULL){
      return NULL;
   }
   for(i=0; i<N; i++){
      diagonal_matrix[i][i] = 1 / pow(diagonal_matrix[i][i], 0.5);
   }
   tmp_W = matrix_multiply(diagonal_matrix, N, N, similarity_matrix, N, N);
   W = matrix_multiply(tmp_W, N, N, diagonal_matrix, N, N);
   free_matrix(tmp_W, N);
   free_matrix(similarity_matrix, N);
   free_matrix(diagonal_matrix, N);
   return W;
}


double euclidean_distance(double *datapoint1, double *datapoint2, int num_of_coordinates){
   double sum = 0;
   int i;
   for (i = 0; i < num_of_coordinates; i++){
       sum +=  pow((datapoint1[i] - datapoint2[i]), 2);
   }
   return sqrt(sum);
}


void free_matrix(double** matrix, int num_of_rows){ /* Free memory of a matrix*/
   int i;
   for(i=0; i<num_of_rows; i++){
      free(matrix[i]);
   }
   free(matrix);
}


void print_matrix(double** matrix, int num_of_rows, int num_of_columns){
   int i;
   int j;
   for(i=0; i<num_of_rows; i++){
      for(j=0; j<num_of_columns; j++){
         printf("%.4f", matrix[i][j]);
            if (j < num_of_columns - 1){
                printf(",");}
      }
      printf("\n");
   }
}


double** matrix_multiply(double** mat1, int ROWS1, int COLS1, double** mat2, int ROWS2, int COLS2){
   int i;
   int j;
   int k;
   double** result = allocate_matrix(ROWS1, COLS2, 0);
   if (COLS1 != ROWS2 || mat1 == NULL || mat2 == NULL || result == NULL){
      return NULL;
   }
   for(i=0; i<ROWS1; i++){
      for(j=0; j<COLS2;j++){
         for(k=0; k<COLS1; k++){
            result[i][j] += mat1[i][k] * mat2[k][j];
         }
      }
   }
   return result;
}


double** update_H(double **H, double** W, int rows, int cols){
   int i;
   int j;
   double** WxH; 
   double** triple_H;
   double** transposed_matrix;
   double** HxHt;
   double** new_H = allocate_matrix(rows, cols, 1);

   WxH = matrix_multiply(W,rows, rows, H, rows, cols);  
   transposed_matrix =  transpose_matrix(H, rows, cols);
   HxHt = matrix_multiply(H, rows, cols, transposed_matrix, cols, rows);
   triple_H = matrix_multiply(HxHt, rows, rows, H, rows, cols);
   if (triple_H == NULL || WxH == NULL || new_H == NULL){
      return NULL;
   }
   for(i=0; i<rows;i++){
      for(j=0; j<cols; j++){
         new_H[i][j] = H[i][j] * (0.5 + 0.5 * (WxH[i][j] / triple_H[i][j]));
      }
   }
   free_matrix(WxH, rows);
   free_matrix(transposed_matrix, cols);
   free_matrix(HxHt, rows);
   free_matrix(triple_H, rows);
   return new_H;
}


double** transpose_matrix(double** matrix, int rows, int cols){
   int i; 
   int j;
   double** transposed_matrix = allocate_matrix(cols, rows, 1);
   if (!transposed_matrix){
      return NULL;
   }
   for(i=0; i<cols; i++){
      for(j=0; j<rows; j++){
         transposed_matrix[i][j] = matrix[j][i];
      }
   }
   return transposed_matrix;
}


double frobenius_norm(double** H, int rows, int cols,double** new_H){
   int i;
   int j;
   double result = 0;
   double** subtracted_matrix = subtract_matrix(new_H, rows, cols, H, rows, cols);
   if (!subtracted_matrix){
      return -1;
   }
   for(i=0; i<rows; i++){
      for(j=0; j<cols; j++){
         result += pow(subtracted_matrix[i][j], 2);
      }
   }
   free_matrix(subtracted_matrix, rows);
   return sqrt(result);
}


double** subtract_matrix(double** mat1, int rows1, int cols1, double** mat2, int rows2, int cols2){
   int i;
   int j;
   double** result = allocate_matrix(rows1, cols1, 1);
   if (rows1 != rows2 || cols1 != cols2 || result == NULL){
      return NULL;
   }
   for(i=0; i<rows1; i++){
      for (j=0; j<cols1; j++){
         result[i][j] = mat1[i][j] - mat2[i][j];
      }
      
   }
   return result;
}


double** symnmf_c(double** H, double** W, int N, int k){
   double epsilon = 0.0001;
   int iterations = 300;
   double** new_H;
   while (iterations){
      new_H = update_H(H, W, N, k);
      if (new_H == NULL){
         return NULL;
      }
      if (pow(frobenius_norm(H, N, k, new_H), 2) < epsilon){
         iterations = 1;
      }
      free_matrix(H, N);
      H = new_H;
      iterations--;
   }
   return H;
}


double** allocate_matrix(int rows, int cols, int command){ /*Allocate memory, command == 0  means calloc, otherwise malloc */
   int i;
   double** result = (double**)malloc(rows * sizeof(double*));
   if (!result){
      return NULL;
   }
   for(i=0; i< rows; i++){
      if (command){
         result[i] = (double*) malloc(cols * sizeof(double));
      }
      else{
         result[i] = (double*) calloc(cols, sizeof(double));
      }
      if (!result[i]){
         return NULL;
      }
   }
   return result;
}


