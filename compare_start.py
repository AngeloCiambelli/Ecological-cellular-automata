import numpy as np
import matplotlib.pyplot as plt
from scipy.ndimage import label
from collections import deque

class Percolation:
    def __init__(self, size, p, seed=None):
        self.size = size
        self.p = p
        if seed is not None:
            np.random.seed(seed)
        self.grid = np.random.rand(size, size) < p
        self.connected, self.num_clusters = label(self.grid)
    
    def plot_percolation(self):
        plt.imshow(self.grid, cmap='gray', interpolation='nearest')
        plt.title('Percolation with p = {}'.format(self.p))
        plt.show()

        plt.imshow(self.connected, cmap='viridis', interpolation='nearest')
        plt.title('Clusters')
        plt.colorbar(label='Cluster Index')
        plt.show()

def cluster_size(perc, cluster_index):
    return np.sum(perc.connected == cluster_index)

def mean_cluster_size(perc):
    cluster_sizes = []
    for i in range(1, perc.num_clusters + 1):
        cluster_sizes.append(cluster_size(perc, i))
    return np.mean(cluster_sizes)

def sample_clusters(perc, n):
    sampled_clusters = set()  # Using a set to ensure uniqueness
    sampled_sites = set()

    def get_neighbors(i, j):
        neighbors = []
        if i > 0: neighbors.append((i-1, j))
        if i < perc.size - 1: neighbors.append((i+1, j))
        if j > 0: neighbors.append((i, j-1))
        if j < perc.size - 1: neighbors.append((i, j+1))
        return neighbors

    while len(sampled_sites) < n:
        i, j = np.random.randint(perc.size), np.random.randint(perc.size)
        if (i, j) not in sampled_sites:
            sampled_sites.add((i, j))
            if perc.connected[i, j] != 0:
                cluster_index = perc.connected[i, j]
                if cluster_index not in sampled_clusters:
                    sampled_clusters.add(cluster_index)
                    queue = deque([(i, j)])
                    while queue:
                        ci, cj = queue.popleft()
                        for ni, nj in get_neighbors(ci, cj):
                            if perc.connected[ni, nj] == cluster_index and (ni, nj) not in sampled_sites:
                                sampled_sites.add((ni, nj))
                                queue.append((ni, nj))

    num_sampled_clusters = len(sampled_clusters)
    average_cluster_size = np.mean([cluster_size(perc, index) for index in sampled_clusters]) if num_sampled_clusters > 0 else 0
    return num_sampled_clusters, average_cluster_size

def sample_clusters_last_row(perc):
    sampled_clusters = set()  # Using a set to ensure uniqueness
    sampled_sites = set()

    def get_neighbors(i, j):
        neighbors = []
        if i > 0: neighbors.append((i-1, j))
        if i < perc.size - 1: neighbors.append((i+1, j))
        if j > 0: neighbors.append((i, j-1))
        if j < perc.size - 1: neighbors.append((i, j+1))
        return neighbors

    for j in range(perc.size):
        if (perc.size - 1, j) not in sampled_sites:
            sampled_sites.add((perc.size - 1, j))
            if perc.connected[perc.size - 1, j] != 0:
                cluster_index = perc.connected[perc.size - 1, j]
                if cluster_index not in sampled_clusters:
                    sampled_clusters.add(cluster_index)
                    queue = deque([(perc.size - 1, j)])
                    while queue:
                        ci, cj = queue.popleft()
                        for ni, nj in get_neighbors(ci, cj):
                            if perc.connected[ni, nj] == cluster_index and (ni, nj) not in sampled_sites:
                                sampled_sites.add((ni, nj))
                                queue.append((ni, nj))

    num_sampled_clusters = len(sampled_clusters)
    average_cluster_size = np.mean([cluster_size(perc, index) for index in sampled_clusters]) if num_sampled_clusters > 0 else 0
    return num_sampled_clusters, average_cluster_size

# Parameters
ps = [0.58, 0.59, 0.5927, 0.6, 0.61] #Critique
#ps = [0.1, 0.2, 0.3, 0.4] #Sous critique
#ps = [0.7, 0.8, 0.9, 1] #Sur critique

num_iterations = 1000
size = 1000
n = size

# Results storage
results_random_sampling = np.zeros((len(ps), num_iterations, 2))
results_last_row_sampling = np.zeros((len(ps), num_iterations, 2))

# Loop through p values
for j, p in enumerate(ps):
    for k in range(num_iterations):
        seed = k  # Use a different seed for each iteration
        perc = Percolation(size, p, seed=seed)
        
        # Random sampling
        num_sampled_clusters, avg_cluster_size = sample_clusters(perc, n)
        results_random_sampling[j, k, 0] = num_sampled_clusters
        results_random_sampling[j, k, 1] = avg_cluster_size

        # Sampling last row
        num_sampled_clusters_last_row, avg_cluster_size_last_row = sample_clusters_last_row(perc)
        results_last_row_sampling[j, k, 0] = num_sampled_clusters_last_row
        results_last_row_sampling[j, k, 1] = avg_cluster_size_last_row

# Plotting
fig, axes = plt.subplots(2, len(ps), figsize=(20, 10), sharey='row')

for j, p in enumerate(ps):
    # Plotting number of clusters visited
    axes[0, j].boxplot([results_random_sampling[j, :, 0], results_last_row_sampling[j, :, 0]], labels=['Random', 'Last Row'])
    axes[0, j].set_title('p = {}'.format(p))
    if j == 0:
        axes[0, j].set_ylabel('Number of Clusters')
    
    # Plotting average cluster size
    axes[1, j].boxplot([results_random_sampling[j, :, 1], results_last_row_sampling[j, :, 1]], labels=['Random', 'Last Row'])
    if j == 0:
        axes[1, j].set_ylabel('Average Cluster Size')

plt.tight_layout()
plt.show()




'''
# Example usage
size = 1000
p = 0.5927
perc = Percolation(size, p)
#perc.plot_percolation()
#print("Number of clusters:", perc.num_clusters)

# Example usage
mean_size = mean_cluster_size(perc)
print("Mean size of clusters:", mean_size)

# Example usage
n = 200
num_sampled_clusters, avg_cluster_size = sample_clusters(perc, n)
print("Number of sampled clusters:", num_sampled_clusters)
print("Average size of sampled clusters:", avg_cluster_size)

# Example usage
num_sampled_clusters_last_row, avg_cluster_size_last_row = sample_clusters_last_row(perc)
print("Number of sampled clusters in the last row:", num_sampled_clusters_last_row)
print("Average size of sampled clusters in the last row:", avg_cluster_size_last_row)
'''


