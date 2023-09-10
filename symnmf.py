import sys
import numpy as np
import SymNMF
import math


def main():
    np.random.seed(0)
    k = int(sys.argv[1])
    goal = sys.argv[2]
    filepath = sys.argv[3]
    X = readVectors(filepath)
    if len(X) == 0:
        return 0
    result = None
    if goal == 'sym':
        result = SymNMF.sym(X)
    elif goal == 'ddg':
        result = SymNMF.ddg(X)
    elif goal == 'norm':
        result = SymNMF.norm(X)
    elif goal == 'symnmf':
        W = SymNMF.norm(X)
        H = initial_H(W, k)
        result = SymNMF.symnmf(H, W, len(X), k)
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
