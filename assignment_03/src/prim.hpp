#ifndef PRIM_HPP
#define PRIM_HPP

#include <vector>
#include <iostream>
#include "../../assignment_01/src/csr.hpp"
#include "kruskal.hpp"

MSTResult run_prim(const CSRGraph &graph, int start_node = 0);

void print_prim_result(const MSTResult &res);

#endif