# Software Project - Clustering Algorithms

## Introduction
This project involves implementing a clustering algorithm using Symmetric Non-negative Matrix Factorization (SymNMF). We also built a K-means clustering algorithm for comparison on various datasets. A highlight of our project is a custom Python library, written in C, aimed at enhancing clustering performance.

## Requirements
- Python 3.x
- C Compiler
- Numpy
- Pandas

## 📂 Structure
- symnmf.py : Python implementation of SymNMF  
- symnmf.c : C implementation of SymNMF  
- symnmf.h : C header file  
- symnmfmodule.c : Python C API Wrapper for SymNMF  
- analysis.py : Analysis and comparison script  
- setup.py : Build script for Python C extension  
- Makefile : Make script for C executable  
- Text_file_input_example.txt Data points input file example

## 🚀 Quick Start

Follow these steps to get your clustering project up and running:

### 1️⃣ Compile C Code for SymNMF
Open your terminal and execute: 
  
make


### 2️⃣ Build Python Extension
In the terminal, run:    
  
python3 setup.py build_ext --inplace

### 3️⃣ Execute the Python Program
Run the SymNMF algorithm with:  

python3 symnmf.py `<k>` `<goal>` `<file_name.txt>`  
Where:
- `<k>`: Specifies the number of clusters
- `<goal>`: Defines the algorithm goal (choices are symnmf, sym, ddg, norm)
- `<file_name.txt>`: Data points input file (e.g., Text_file_input_example.txt)

Text File Input Format:  
Such as Text_file_input_example.txt, should contain vectors separated by commas. Each line in the file represents a single data point in the feature space.  

### 4️⃣ Conduct Analysis
To perform the data analysis, execute:  
python3 analysis.py <k> <file_name.txt>



## Contributors
- Lior Kovtun
- Shalev Baruch
