import os
import random

# Directories for output test files
TEST_DIR = "assignment_04/tests"
os.makedirs(TEST_DIR, exist_ok=True)


# ----------------------------------------------------------------------
# 1. Vertex Coloring Input Generator
# Format: V E \n u degree n1 n2 ...
# Undirected, unweighted graph. No self-loops.
# ----------------------------------------------------------------------
def generate_vertex_coloring_input(filepath, V, target_E):
    adj = {i: set() for i in range(V)}
    edges_added = 0

    # Ensure sparse random edges (degree ~ 2V to 4V as required)
    attempts = 0
    max_attempts = target_E * 10
    while edges_added < target_E and attempts < max_attempts:
        u = random.randint(0, V - 1)
        v = random.randint(0, V - 1)
        if u != v and v not in adj[u]:
            adj[u].add(v)
            adj[v].add(u)
            edges_added += 1
        attempts += 1

    with open(filepath, "w") as f:
        f.write(f"{V} {edges_added}\n")
        for u in range(V):
            neighbors = sorted(list(adj[u]))
            degree = len(neighbors)
            line = f"{u} {degree}"
            if degree > 0:
                line += " " + " ".join(map(str, neighbors))
            f.write(line + "\n")


# ----------------------------------------------------------------------
# 2. PageRank Input Generator
# Format: V E \n u outdegree n1 n2 ... \n DAMPING d \n TOLERANCE eps \n MAX_ITERATIONS n
# Directed, unweighted graph.
# ----------------------------------------------------------------------
def generate_pagerank_input(filepath, V, target_E, damping=0.85, tol=0.0001, max_iter=100):
    adj = {i: set() for i in range(V)}
    edges_added = 0

    # Ensure every vertex has at least one outgoing edge to avoid isolated nodes where possible
    for u in range(V):
        v = random.randint(0, V - 1)
        adj[u].add(v)
        edges_added += 1

    attempts = 0
    max_attempts = target_E * 10
    while edges_added < target_E and attempts < max_attempts:
        u = random.randint(0, V - 1)
        v = random.randint(0, V - 1)
        if v not in adj[u]:
            adj[u].add(v)
            edges_added += 1
        attempts += 1

    with open(filepath, "w") as f:
        f.write(f"{V} {edges_added}\n")
        for u in range(V):
            neighbors = sorted(list(adj[u]))
            outdeg = len(neighbors)
            line = f"{u} {outdeg}"
            if outdeg > 0:
                line += " " + " ".join(map(str, neighbors))
            f.write(line + "\n")
        f.write(f"DAMPING {damping}\n")
        f.write(f"TOLERANCE {tol}\n")
        f.write(f"MAX_ITERATIONS {max_iter}\n")


# ----------------------------------------------------------------------
# 3. K-Means Clustering Input Generator
# Format: N D K \n x1 x2 ... xD \n MAX_ITERATIONS n \n TOLERANCE eps
# ----------------------------------------------------------------------
def generate_kmeans_input(filepath, N, D, K, max_iter=300, tol=0.0001):
    with open(filepath, "w") as f:
        f.write(f"{N} {D} {K}\n")
        for _ in range(N):
            coords = [round(random.uniform(0.0, 100.0), 4) for _ in range(D)]
            f.write(" ".join(map(str, coords)) + "\n")
        f.write(f"MAX_ITERATIONS {max_iter}\n")
        f.write(f"TOLERANCE {tol}\n")


# ----------------------------------------------------------------------
# 4. FastMap Input Generator
# Format: N k \n full symmetric distance matrix (NxN) with 0 diagonal
# ----------------------------------------------------------------------
def generate_fastmap_input(filepath, N, target_k):
    # Generate random coordinates in k-D space to guarantee valid metric space (triangle inequality)
    coords = [[random.uniform(0.0, 50.0) for _ in range(target_k)] for _ in range(N)]
    
    # Calculate Euclidean distance matrix
    dist_matrix = [[0.0] * N for _ in range(N)]
    for i in range(N):
        for j in range(i + 1, N):
            dist = sum((coords[i][d] - coords[j][d]) ** 2 for d in range(target_k)) ** 0.5
            dist_matrix[i][j] = round(dist, 4)
            dist_matrix[j][i] = dist_matrix[i][j]

    with open(filepath, "w") as f:
        f.write(f"{N} {target_k}\n")
        for row in dist_matrix:
            f.write(" ".join(map(str, row)) + "\n")


# ======================================================================
# Main Execution: Generate All Required Assignment Datasets
# ======================================================================
def main():
    print("Generating input files for Assignment 4...")

    # --- 1. Vertex Coloring Tests (Undirected CSR inputs) ---
    # vc_configs = [
    #     ("color_10.txt", 10, 20),
    #     ("color_100.txt", 100, 300),
    #     ("color_10000.txt", 10000, 30000),
    #     ("color_50000.txt", 50000, 150000),
    #     ("color_100000.txt", 100000, 300000),
    # ]
    # for filename, V, E in vc_configs:
    #     path = os.path.join(TEST_DIR, filename)
    #     generate_vertex_coloring_input(path, V, E)
    #     print(f"Created: {path}")

    # --- 2. PageRank Tests (Directed CSR inputs) ---
    # pr_configs = [
    #     ("pagerank_10.txt", 10, 20),
    #     ("pagerank_100.txt", 100, 300),
    #     ("pagerank_1000.txt", 1000, 3000),
    #     ("pagerank_10000.txt", 10000, 30000),
    #     ("pagerank_50000.txt", 50000, 150000),
    # ]
    # for filename, V, E in pr_configs:
    #     path = os.path.join(TEST_DIR, filename)
    #     generate_pagerank_input(path, V, E)
    #     print(f"Created: {path}")

    # --- 3. K-Means Tests ---
    # km_configs = [
    #     ("km_01.txt", 100, 2, 3),
    #     ("km_02.txt", 1000, 2, 5),
    #     ("km_03.txt", 10000, 5, 8),
    #     ("km_04.txt", 100000, 5, 10),
    # ]
    # for filename, N, D, K in km_configs:
    #     path = os.path.join(TEST_DIR, filename)
    #     generate_kmeans_input(path, N, D, K)
    #     print(f"Created: {path}")

    # --- 4. FastMap Tests ---
    fm_configs = [
        ("fm_01.txt", 10, 2),
        ("fm_02.txt", 100, 2),
        ("fm_03.txt", 1000, 3),
        ("fm_04.txt", 10000, 3),
    ]
    for filename, N, target_k in fm_configs:
        path = os.path.join(TEST_DIR, filename)
        generate_fastmap_input(path, N, target_k)
        print(f"Created: {path}")

    print("\nAll input files successfully generated in 'assignment_04/tests/'.")


if __name__ == "__main__":
    main()