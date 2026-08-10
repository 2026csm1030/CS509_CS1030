# CS509_CS1030

## Student Details
- **Name:** Prashik Meshram
- **Entry Number:** 2026CSM1030
- **Repository Type:** Individual

## Language and Environment
- **Language:** C++ (C++17)
- **Compiler:** g++ (Ubuntu 9.4.0-1ubuntu1~20.04.2) 9.4.0 — WSL Ubuntu 20.04
- **Compilation flags:** `-std=c++17 -O2`
- **OS / Machine:** [Fill in: e.g. WSL2, Windows 11 host, CPU model, RAM] — run `uname -a` and `lscpu` on your machine and paste the relevant lines here.
- **Timing method:** `std::chrono::steady_clock`, measured in milliseconds via `std::chrono::duration<double, std::milli>`.

## Common Wrapper: Build and Usage

The common wrapper provides an interactive menu interface to compile and execute any submitted assignment module and its associated test cases.

### Compilation
From the repository root directory, compile the wrapper using:

```bash
g++ -std=c++17 -O2 -o common_wrapper common_wrapper/wrapper.cpp
```

### Execution

Run the executable from the repository root:

```bash
./common_wrapper
```

The menu allows you to:

1. List available assignments.
2. Compile a selected assignment driver.
3. Execute a single test file for a selected assignment.
4. Execute all test files for a selected assignment (`--all`).
5. Compile and run all assignments across all test suites.