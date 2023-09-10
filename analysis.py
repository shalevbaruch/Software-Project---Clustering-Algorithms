import sys
import numpy as np
import math
from sklearn.metrics import silhouette_score, pairwise_distances
from SymNMF import symnmf, norm
from symnmf import initial_H, readVectors 


def distance(vectorA, vectorB):  # euclidean distance
    return math.sqrt(sum((a - b) ** 2 for (a, b) in zip(vectorA, vectorB)))


def findClusterIndex(centroidsArray, vector):
    return min(range(len(centroidsArray)), key=lambda i: distance(centroidsArray[i], vector))


def KMeansAlgorithm(datapoints_arr, k, iteration):
    epsilon=0.0001
    i = 0
    centroidsArray = datapoints_arr[:k]
    num_of_coordinates = len(centroidsArray[0])
    while i < iteration:
        datapoints_num_in_centroids = [0 for i in range (k)]
        newCentroidsArray = [[0] * num_of_coordinates for _ in range(k)]
        for vector in datapoints_arr:
            index = findClusterIndex(centroidsArray, vector)
            for j in range(num_of_coordinates):
                newCentroidsArray[index][j] += vector[j]
            datapoints_num_in_centroids[index] += 1

        for m in range(k):
            centroid = newCentroidsArray[m]
            if datapoints_num_in_centroids[m] == 0:
                newCentroidsArray[m] = centroidsArray[m]
            else:
                newCentroidsArray[m] = [coordinate / datapoints_num_in_centroids[m] for coordinate in centroid]
        
        to_exit = all(distance(a, b) < epsilon for (a, b) in zip(centroidsArray, newCentroidsArray))
        centroidsArray = newCentroidsArray
        if to_exit:
            break
        i += 1
    return centroidsArray






def main():
    np.random.seed(0)
    iterations = 300
    k = int(sys.argv[1])
    X = readVectors(sys.argv[2])
    kmeans_clusters = KMeansAlgorithm(X, k, iterations)
    distances = pairwise_distances(X, kmeans_clusters)
    kmeans_lables = np.argmin(distances, axis=1)
    kmeans_score = "%.4f" % silhouette_score(X, kmeans_lables)
    
    W = norm(X)
    H = initial_H(W, k)
    final_H = symnmf(H, W, len(X), k)
    symnmf_lables = np.argmax(final_H, axis=1)
    symnmf_score = "%.4f" % silhouette_score(X, symnmf_lables)

    print(f"nmf: {symnmf_score}")
    print(f"kmeans: {kmeans_score}")






if __name__ == "__main__":
    main()