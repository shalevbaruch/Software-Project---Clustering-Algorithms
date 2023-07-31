import sys
import numpy as np
import mysymnmf
import math


def main():
    np.random.seed(0)
    k = int(sys.argv[1])
    goal = sys.argv[2]
    filepath = sys.argv[3]
    X = readVectors(filepath)
    # print_result(X)
    if goal == 'sym':
        print_result(mysymnmf.sym(X))
    elif goal == 'ddg':
        print_result(mysymnmf.ddg(X))
    elif goal == 'norm':
        print_result(mysymnmf.norm(X))
    elif goal == 'symnmf':
        W = mysymnmf.norm(X)
        H = initial_H(W, k)
        print_result(mysymnmf.symnmf(W, k))
    else:
        print("An Error Has Occurred")
        return 1
    return 0
         
    


def initial_H(W, k):
    m = np.mean(W)
    return np.random.uniform(0,2*math.sqrt(m/k), size=(len(W), k))


def print_result(centroidsArray):
    for centroid in centroidsArray:
        line = ",".join("%.4f" % coord for coord in centroid)
        print(line)
    print("")


def readVectors(fileName):
    vectors_numpy = np.genfromtxt(fileName, delimiter=',', dtype=float)
    return vectors_numpy


if __name__ == "__main__":
    main()
