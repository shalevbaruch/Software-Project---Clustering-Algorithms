import sys
import numpy as np
import symnmf
import math


def main():
    np.random.seed(0)
    k = int(sys.argv[1])
    goal = sys.argv[2]
    filepath = sys.argv[3]
    X = readVectors(filepath)
    result = None
    # print_result(X)
    if goal == 'sym':
        result = symnmf.sym(X)
    elif goal == 'ddg':
        result = symnmf.ddg(X)
    elif goal == 'norm':
        result = symnmf.norm(X)
    elif goal == 'symnmf':
        W = symnmf.norm(X)
        H = initial_H(W, k)
        result = symnmf.symnmf(H, W, k)
    if result == None:
        print("An Error Has Occurred")
        return 1
    print_result(result)
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
    vectors = np.genfromtxt(fileName, delimiter=',', dtype=float)
    return vectors


if __name__ == "__main__":
    main()
