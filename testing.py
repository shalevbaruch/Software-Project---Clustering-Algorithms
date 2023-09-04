from sklearn.cluster import KMeans
import sys
import numpy as np
from sklearn.metrics import silhouette_score
from sklearn.decomposition import NMF


def apply_kmeans(X,k):
    kmeans = KMeans(n_clusters=k)
    kmeans.fit(X)
    labels = kmeans.labels_
    return labels


def readVectors(fileName):
    vectors = np.genfromtxt(fileName, delimiter=',', dtype=float)
    return vectors


def main():
    k = int(sys.argv[1])
    X = readVectors(sys.argv[2])
    kmeans_labels = apply_kmeans(X,k)
    kmeans_score = silhouette_score(X, kmeans_labels)
    print(kmeans_score)
    symnmf_labels = symnmf(X, k)
    symnmf_score = silhouette_score(X, symnmf_labels)
    print(f"nmf: {symnmf_score:.4f}")


def symnmf(X, n_components):
    model = NMF(n_components=n_components, init='random', random_state=0)
    W = model.fit_transform(X)
    labels = np.argmax(W, axis=1)
    return labels


main()