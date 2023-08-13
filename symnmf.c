#define _GNU_SOURCE
#include<stdio.h>
#include <stdlib.h>
#include <math.h>
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


int main(int argc, char* argv[]){
   char* goal = argv[1];
   char* filepath = argv[2];
   int num_of_coordinates;
   int N;
   double** X = NULL;
   double** result;
   if (initialize(&X, &num_of_coordinates, &N, filepath) != 0){
        printf("An Error Has Occurred");
        return 1;
   }
   if (goal == "sym"){
      if ((result = sym_c(X,num_of_coordinates,N)) == NULL){
         printf("An Error Has Occurred");
         return 1;
      }
   }
   else if (goal == "ddg"){
      if ((result = ddg_c(X,num_of_coordinates,N)) == NULL){
         printf("An Error Has Occurred");
         return 1;
      }
   }
   else if (goal == "norm"){
      if ((result = norm_c(X,num_of_coordinates,N)) == NULL){
         printf("An Error Has Occurred");
         return 1;
      }
   }
   print_matrix(result, N, num_of_coordinates);
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
         if (line[i] == ',') { /* if we find a comma, increment the count */
               num_of_coordinates++;
         }
      }
      }
      if (num_of_datapoints == capacity){
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
   return 0; 
}


void enter_datapoint(double *arr, char* line){
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


double** sym_c(double** X, int num_of_coordinates, int N){ // X is a matrix of size N*num_of_coordinates
   int i;
   int j;
   double sim_value;
   double** similarity_matrix = (double**)malloc(N * sizeof(double*));
   if (similarity_matrix == NULL){
      return NULL;
   }

   for (int i = 0; i < N; i++) {
      similarity_matrix[i] = (double*)malloc(N * sizeof(double));
      if (similarity_matrix[i] == NULL) {
         return NULL;
      }

      for (int j = 0; j < i; j++) {
         double dist_squared = euclidean_distance(X[i], X[j], num_of_coordinates);
         double sim_value = exp(-dist_squared * dist_squared / 2);
         similarity_matrix[i][j] = sim_value;
         similarity_matrix[j][i] = sim_value; // Copy to the lower triangular part (symmetric property)
      }

      similarity_matrix[i][i] = 0;
   }
   return similarity_matrix;
}


double** ddg_c(double** X, int num_of_coordinates, int N){
   double** diagonal_matrix;
   int i;
   int j;
   double row_sum;
   double** similarity_matrix = sym_c(X, num_of_coordinates, N);
   if (similarity_matrix == NULL){
      return NULL;
   }
   diagonal_matrix = (double**)malloc(N*sizeof(double*));
   if (diagonal_matrix == NULL){
      return NULL;
   }
   for(i=0; i<N; i++){
      row_sum = 0;
      diagonal_matrix[i] = calloc(N, sizeof(double));
      if (diagonal_matrix[i] == NULL){
         return NULL;
      }
      for(j=0; j<N; j++){
         row_sum += similarity_matrix[i][j];
      }
      diagonal_matrix[i][i] = row_sum;
   }
   free_matrix(similarity_matrix, N);
   return diagonal_matrix;
}


double** norm_c(double** X, int num_of_coordinates, int N){
   double** similarity_matrix = sym_c(X, num_of_coordinates, N);
   double** diagonal_matrix = ddg_c(X, num_of_coordinates, N);
   double** W;
   int i; int j;
   if (similarity_matrix == NULL || diagonal_matrix == NULL){
      return NULL;
   }
   for(i=0; i<N; i++){
      diagonal_matrix[i][i] = 1 / pow(diagonal_matrix[i][i], 0.5);
   }
   W = matrix_multiply(diagonal_matrix, N, N, similarity_matrix, N, N);
   W = matrix_multiply(W, N, N, diagonal_matrix, N, N);
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


void free_matrix(double** matrix, int num_of_rows){
   int i;
   int j;
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
   printf("\n");
}

double** matrix_multiply(double** mat1, int ROWS1, int COLS1, double** mat2, int ROWS2, int COLS2){
   int i;
   int j;
   int k;
   if (COLS1 != ROWS2){
      return NULL;
   }
   double** result = (double**)malloc(ROWS1*sizeof(double*));
   for(i=0; i<ROWS1; i++){
      result[i] = (double*)calloc(COLS2, sizeof(double));
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


