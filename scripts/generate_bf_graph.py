#!/usr/bin/env python3
import random
import sys

def generate_directed_bf(V, avg_degree, min_weight=-5, max_weight=20, allow_neg_cycles=False, seed=42):
    random.seed(seed)
    edges = {}
    target_edges = int(V * avg_degree)

    # Topological structure guarantees NO negative cycles unless explicitly allowed
    for u in range(V):
        degree = max(1, int(avg_degree))
        for _ in range(degree):
            v = random.randint(0, V - 1)
            if u == v:
                continue
            
            # If DAG structure (u < v), weights can safely be negative without forming cycles
            if not allow_neg_cycles and u > v:
                w = random.randint(1, max_weight)
            else:
                w = random.randint(min_weight, max_weight)
                
            edges[(u, v)] = w

    adj = [[] for _ in range(V)]
    for (u, v), w in edges.items():
        adj[u].append((v, w))

    return adj, len(edges)

def write_bf_file(path, V, E, adj, source=0):
    with open(path, 'w') as f:
        f.write(f"{V} {E}\n")
        for u in range(V):
            neighbors = adj[u]
            parts = [str(u), str(len(neighbors))]
            for (v, w) in neighbors:
                parts.extend([str(v), str(w)])
            f.write(" ".join(parts) + "\n")
        f.write(f"SOURCE {source}\n")

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: python3 generate_bf_graph.py <V> <avg_degree> <output_file> [source]")
        sys.exit(1)

    V = int(sys.argv[1])
    avg_degree = float(sys.argv[2])
    out_path = sys.argv[3]
    source = int(sys.argv[4]) if len(sys.argv) > 4 else 0

    adj, E = generate_directed_bf(V, avg_degree)
    write_bf_file(out_path, V, E, adj, source)
    print(f"Generated Bellman-Ford graph {out_path}: V={V}, E={E}, Source={source}")