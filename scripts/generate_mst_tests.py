#!/usr/bin/env python3
import os
import random

def generate_connected_graph(V, target_E, min_weight=1, max_weight=20):
    """
    Generates a connected, undirected sparse graph.
    First creates a spanning tree to guarantee connectivity,
    then adds random edges until target_E is reached.
    """
    edges = set()
    
    # Step 1: Create a random spanning tree (V-1 edges) to ensure connectedness
    nodes = list(range(V))
    random.shuffle(nodes)
    
    for i in range(1, V):
        u = nodes[i]
        v = nodes[random.randint(0, i - 1)]
        u, v = min(u, v), max(u, v)
        w = random.randint(min_weight, max_weight)
        edges.add((u, v, w))
    
    # Step 2: Add remaining random edges up to target_E
    edge_pairs = set((u, v) for u, v, _ in edges)
    while len(edges) < target_E:
        u = random.randint(0, V - 1)
        v = random.randint(0, V - 1)
        if u != v:
            u, v = min(u, v), max(u, v)
            if (u, v) not in edge_pairs:
                edge_pairs.add((u, v))
                w = random.randint(min_weight, max_weight)
                edges.add((u, v, w))
                
    # Step 3: Build bidirectional adjacency list
    adj = {i: [] for i in range(V)}
    for u, v, w in edges:
        adj[u].append((v, w))
        adj[v].append((u, w))
        
    for i in range(V):
        adj[i].sort(key=lambda x: x[0])
        
    return adj, len(edges)

def write_graph_file(filepath, V, actual_E, adj):
    with open(filepath, "w") as f:
        # Header: V and E
        f.write(f"{V} {actual_E}\n")
        # Node rows: u degree n1 w1 n2 w2 ...
        for u in range(V):
            degree = len(adj[u])
            line_parts = [str(u), str(degree)]
            for neighbor, weight in adj[u]:
                line_parts.extend([str(neighbor), str(weight)])
            f.write(" ".join(line_parts) + "\n")

def main():
    output_dir = "assignment_03/tests"
    os.makedirs(output_dir, exist_ok=True)
    
    # Required sizes from Assignment 3 specifications
    test_configs = [
        ("mst_10.txt", 10, 20),
        ("mst_100.txt", 100, 250),
        ("mst_10000.txt", 10000, 25000),
        ("mst_50000.txt", 50000, 125000),
        ("mst_100000.txt", 100000, 250000),
    ]
    
    # Also create test_01.txt matching the example in Section 5.2 of the spec
    example_path = os.path.join(output_dir, "mst_example.txt")
    with open(example_path, "w") as f:
        f.write("5 7\n")
        f.write("0 2 1 2 3 6\n")
        f.write("1 4 0 2 2 3 3 8 4 5\n")
        f.write("2 2 1 3 4 7\n")
        f.write("3 3 0 6 1 8 4 9\n")
        f.write("4 3 1 5 2 7 3 9\n")
    print(f"Generated spec example: {example_path}")

    for filename, V, target_E in test_configs:
        print(f"Generating {filename} (V={V}, E={target_E})...")
        adj, actual_E = generate_connected_graph(V, target_E)
        out_path = os.path.join(output_dir, filename)
        write_graph_file(out_path, V, actual_E, adj)
        print(f"  -> Successfully written to {out_path}")

if __name__ == "__main__":
    main()