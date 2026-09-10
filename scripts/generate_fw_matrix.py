#!/usr/bin/env python3
import random
import sys

def generate_fw_matrix(V, density=0.4, min_weight=-5, max_weight=20, seed=42):
    random.seed(seed)
    matrix = [["INF" for _ in range(V)] for _ in range(V)]

    for i in range(V):
        matrix[i][i] = "0"

    for i in range(V):
        for j in range(V):
            if i != j and random.random() < density:
                # Keep weights positive for i > j to avoid negative cycles
                w = random.randint(1, max_weight) if i > j else random.randint(min_weight, max_weight)
                matrix[i][j] = str(w)

    return matrix

def write_fw_file(path, V, matrix):
    with open(path, 'w') as f:
        f.write(f"{V}\n")
        for row in matrix:
            f.write(" ".join(row) + "\n")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python3 generate_fw_matrix.py <V> <output_file> [density]")
        sys.exit(1)

    V = int(sys.argv[1])
    out_path = sys.argv[2]
    density = float(sys.argv[3]) if len(sys.argv) > 3 else 0.4

    matrix = generate_fw_matrix(V, density)
    write_fw_file(out_path, V, matrix)
    print(f"Generated Floyd-Warshall matrix {out_path}: V={V}")