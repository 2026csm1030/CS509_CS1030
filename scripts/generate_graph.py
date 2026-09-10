#!/usr/bin/env python3
"""
Generates a random connected undirected graph in the CS509 adjacency-list
format, for BFS/DFS (unweighted) or SSSP (weighted, positive) test files.

Usage:
    python3 generate_graph.py <V> <avg_degree> <output_file> [--weighted]

Guarantees connectivity by first building a random spanning tree over all V
vertices, then adding extra random edges until the target edge count
(V * avg_degree / 2) is reached.
"""
import random
import sys

def generate(V, avg_degree, weighted, seed=42):
    random.seed(seed)
    edges = set()

    # Step 1: random spanning tree guarantees the graph is connected.
    vertices = list(range(V))
    random.shuffle(vertices)
    for i in range(1, V):
        u = vertices[i]
        v = vertices[random.randint(0, i - 1)]
        a, b = min(u, v), max(u, v)
        edges.add((a, b))

    # Step 2: add extra random edges up to the target count.
    target_edges = max(V - 1, int(V * avg_degree / 2))
    attempts = 0
    max_attempts = target_edges * 20 + 1000
    while len(edges) < target_edges and attempts < max_attempts:
        u = random.randint(0, V - 1)
        v = random.randint(0, V - 1)
        attempts += 1
        if u == v:
            continue
        a, b = min(u, v), max(u, v)
        edges.add((a, b))

    adj = [[] for _ in range(V)]
    for (a, b) in edges:
        w1 = random.randint(1, 20) if weighted else None
        w2 = random.randint(1, 20) if weighted else None
        if weighted:
            adj[a].append((b, w1))
            adj[b].append((a, w1))  # same weight both directions (undirected)
        else:
            adj[a].append(b)
            adj[b].append(a)

    return edges, adj

def write_file(path, V, edges, adj, weighted, source=0):
    with open(path, 'w') as f:
        f.write(f"{V} {len(edges)}\n")
        for u in range(V):
            neighbors = adj[u]
            if weighted:
                parts = [str(u), str(len(neighbors))]
                for (v, w) in neighbors:
                    parts.append(str(v))
                    parts.append(str(w))
            else:
                parts = [str(u), str(len(neighbors))] + [str(v) for v in neighbors]
            f.write(" ".join(parts) + "\n")
        f.write(f"SOURCE {source}\n")

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print(__doc__)
        sys.exit(1)
    V = int(sys.argv[1])
    avg_degree = float(sys.argv[2])
    out_path = sys.argv[3]
    weighted = "--weighted" in sys.argv

    edges, adj = generate(V, avg_degree, weighted)
    write_file(out_path, V, edges, adj, weighted)
    print(f"Wrote {out_path}: V={V}, E={len(edges)}, weighted={weighted}")