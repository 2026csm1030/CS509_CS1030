// Common Wrapper — CS509 repository-level interface.
//
// Provides one menu/interface for compiling and running the submitted
// assignments. This does NOT replace each assignment's dedicated driver —
// it invokes it. To add a new assignment later, add one entry to the
// `registry()` function below; nothing else in this file needs to change.
//
// Build:
//   g++ -std=c++17 -O2 -o common_wrapper common_wrapper/wrapper.cpp
// Run (from the repository root, so relative paths resolve correctly):
//   ./common_wrapper

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdlib>
#include <algorithm>

using namespace std;

namespace fs = filesystem;

struct AssignmentEntry
{
  string display_name;    // shown in the menu
  vector<string> sources; // all .cpp files needed to build the driver
  string binary_path;     // where the compiled driver goes
  string test_dir;        // folder containing this assignment's test files
  string extra_run_args;  // args appended after the test file/--all path (e.g. block size)
};

// ---- Registry of all assignments the wrapper can build/run ----
static vector<AssignmentEntry> registry()
{
  return {
      {
          "Assignment 01 - GEMM (Simple + Blocking)",
          {"assignment_01/driver/gemm_driver.cpp",
           "assignment_01/src/gemm_simple.cpp",
           "assignment_01/src/gemm_blocking.cpp"},
          "assignment_01/gemm_driver",
          "assignment_01/tests",
          "32" // default block size passed to the driver
      },
      {"Assignment 01 - CSR Conversion",
       {"assignment_01/driver/csr_driver.cpp", "assignment_01/src/csr.cpp"},
       "assignment_01/csr_driver",
       "assignment_01/tests",
       ""},
      {"Assignment 02 - Bellman-Ford Shortest Path",
       {"assignment_02/driver/bellman_ford_driver.cpp",
        "assignment_02/src/bellman_ford.cpp",
        "assignment_01/src/csr.cpp"},
       "assignment_02/bf_driver",
       "assignment_02/tests",
       ""},
      {"Assignment 02 - Floyd-Warshall All-Pairs Shortest Path",
       {"assignment_02/driver/floyd_warshall_driver.cpp",
        "assignment_02/src/floyd_warshall.cpp"},
       "assignment_02/fw_driver",
       "assignment_02/tests",
       ""},
      {"Assignment 03 - Kruskal's MST",
       {"assignment_03/driver/kruskal_driver.cpp",
        "assignment_03/src/kruskal.cpp",
        "assignment_01/src/csr.cpp"},
       "assignment_03/kruskal_driver",
       "assignment_03/tests",
       ""},
      {"Assignment 03 - Prim's MST",
       {"assignment_03/driver/prim_driver.cpp",
        "assignment_03/src/prim.cpp",
        "assignment_01/src/csr.cpp"},
       "assignment_03/prim_driver",
       "assignment_03/tests",
       ""},
      {"Assignment 04 - Vertex Coloring (Graph Coloring)",
       {"assignment_04/driver/vertex_coloring_driver.cpp",
        "assignment_04/src/vertex_coloring.cpp",
        "assignment_01/src/csr.cpp"},
       "assignment_04/vertex_coloring_driver",
       "assignment_04/tests",
       ""},
      {"Assignment 04 - PageRank",
       {"assignment_04/driver/pagerank_driver.cpp",
        "assignment_04/src/pagerank.cpp",
        "assignment_01/src/csr.cpp"},
       "assignment_04/pagerank_driver",
       "assignment_04/tests",
       ""}
      // Example of how a future assignment would be added:
      // {
      //     "Assignment 02 - <Title>",
      //     {"assignment_02/driver/xyz_driver.cpp", "assignment_02/src/xyz.cpp"},
      //     "assignment_02/xyz_driver",
      //     "assignment_02/tests",
      //     ""
      // }
  };
}

static bool file_exists(const string &path)
{
  return fs::exists(path);
}

// Compiles one assignment's driver. Returns true on success.
static bool compile_assignment(const AssignmentEntry &a)
{
  for (const auto &src : a.sources)
  {
    if (!file_exists(src))
    {
      cerr << "Error: required source file not found: " << src << "\n";
      return false;
    }
  }

  string cmd = "g++ -std=c++17 -O2 -o " + a.binary_path;
  for (const auto &src : a.sources)
    cmd += " " + src;

  cout << "Compiling: " << a.display_name << "\n";

  cout << "  " << cmd << "\n";

  int result = system(cmd.c_str());
  if (result != 0)

  {
    cerr << "Error: compilation failed for " << a.display_name << "\n";
    return false;
  }

  cout << "Build succeeded: " << a.binary_path << "\n";

  return true;
}

// Runs the compiled driver on one specific test file.
static bool run_single_test(const AssignmentEntry &a, const string &test_file)
{
  if (!file_exists(a.binary_path))
  {
    cerr << "Error: binary not built yet. Compile " << a.display_name << " first.\n";
    return false;
  }

  string full_path = a.test_dir + "/" + test_file;

  if (!file_exists(full_path))
  {
    cerr << "Error: test file not found: " << full_path << "\n";
    return false;
  }

  string cmd = a.binary_path + " " + full_path + " " + a.extra_run_args;

  return system(cmd.c_str()) == 0;
}

// Runs the compiled driver on every test file for this assignment.
static bool run_all_tests(const AssignmentEntry &a)
{
  if (!file_exists(a.binary_path))
  {
    cerr << "Error: binary not built yet. Compile " << a.display_name << " first.\n";
    return false;
  }

  if (!fs::exists(a.test_dir) || !fs::is_directory(a.test_dir))
  {
    cerr << "Error: test directory not found: " << a.test_dir << "\n";
    return false;
  }

  string cmd = a.binary_path + " --all " + a.test_dir + " " + a.extra_run_args;

  return system(cmd.c_str()) == 0;
}

static void print_assignment_list(const vector<AssignmentEntry> &list)
{
  cout << "\nAvailable assignments:\n";

  for (size_t i = 0; i < list.size(); ++i)
  {
    cout << "  " << (i + 1) << ". " << list[i].display_name << "\n";
  }
}

static int select_assignment(const vector<AssignmentEntry> &list)
{
  print_assignment_list(list);

  cout << "Select assignment number: ";

  int choice;

  if (!(cin >> choice) || choice < 1 || choice > static_cast<int>(list.size()))
  {
    cerr << "Error: invalid selection.\n";
    cin.clear();
    cin.ignore(10000, '\n');

    return -1;
  }

  return choice - 1;
}

static void list_test_files(const AssignmentEntry &a)
{
  cout << "\nTest files in " << a.test_dir << ":\n";

  if (!fs::exists(a.test_dir))
  {
    cerr << "Error: test directory not found: " << a.test_dir << "\n";
    return;
  }

  vector<string> files;

  for (const auto &entry : fs::directory_iterator(a.test_dir))
  {
    if (entry.path().extension() == ".txt")
      files.push_back(entry.path().filename().string());
  }

  sort(files.begin(), files.end());

  for (const auto &f : files)
    cout << "  - " << f << "\n";
}

static void print_menu()
{
  cout << "\n================ CS509 Common Wrapper ================\n";
  cout << "1. List available assignments\n";
  cout << "2. Compile a selected assignment\n";
  cout << "3. Run one test file for a selected assignment\n";
  cout << "4. Run all test files for a selected assignment\n";
  cout << "5. Compile and run ALL submitted assignments (all test files)\n";
  cout << "6. Exit\n";
  cout << "=======================================================\n";
  cout << "Choice: ";
}

int main()
{
  auto assignments = registry();

  while (true)
  {
    print_menu();
    int choice;

    if (!(cin >> choice))
    {
      cerr << "Error: invalid input.\n";
      cin.clear();
      cin.ignore(10000, '\n');
      continue;
    }

    if (choice == 1)
    {
      print_assignment_list(assignments);
    }
    else if (choice == 2)
    {
      int idx = select_assignment(assignments);
      if (idx >= 0)
        compile_assignment(assignments[idx]);
    }
    else if (choice == 3)
    {
      int idx = select_assignment(assignments);
      if (idx >= 0)
      {
        list_test_files(assignments[idx]);
        cout << "Enter test file name (e.g. gemm_test_01.txt): ";
        string fname;
        cin >> fname;
        run_single_test(assignments[idx], fname);
      }
    }
    else if (choice == 4)
    {
      int idx = select_assignment(assignments);
      if (idx >= 0)
        run_all_tests(assignments[idx]);
    }
    else if (choice == 5)
    {
      cout << "\nCompiling and running all submitted assignments...\n";
      for (const auto &a : assignments)
      {
        cout << "\n---- " << a.display_name << " ----\n";
        if (compile_assignment(a))
        {
          run_all_tests(a);
        }
        else
        {
          cerr << "Skipping run for " << a.display_name
               << " due to compilation failure.\n";
        }
      }
    }
    else if (choice == 6)
    {
      cout << "Exiting.\n";
      break;
    }
    else
    {
      cerr << "Error: invalid choice. Please select 1-6.\n";
    }
  }

  return 0;
}