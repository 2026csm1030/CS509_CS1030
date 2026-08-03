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

namespace fs = std::filesystem;

// Describes one assignment the wrapper knows how to build and run.
struct AssignmentEntry
{
  std::string display_name;         // shown in the menu
  std::vector<std::string> sources; // all .cpp files needed to build the driver
  std::string binary_path;          // where the compiled driver goes
  std::string test_dir;             // folder containing this assignment's test files
  std::string extra_run_args;       // args appended after the test file/--all path (e.g. block size)
};

// ---- Registry of all assignments the wrapper can build/run ----
// Add new assignments here as they are completed during the semester.
static std::vector<AssignmentEntry> registry()
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
      }
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

static bool file_exists(const std::string &path)
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
      std::cerr << "Error: required source file not found: " << src << "\n";
      return false;
    }
  }

  std::string cmd = "g++ -std=c++17 -O2 -o " + a.binary_path;
  for (const auto &src : a.sources)
    cmd += " " + src;

  std::cout << "Compiling: " << a.display_name << "\n";
  std::cout << "  " << cmd << "\n";
  int result = std::system(cmd.c_str());
  if (result != 0)
  {
    std::cerr << "Error: compilation failed for " << a.display_name << "\n";
    return false;
  }
  std::cout << "Build succeeded: " << a.binary_path << "\n";
  return true;
}

// Runs the compiled driver on one specific test file.
static bool run_single_test(const AssignmentEntry &a, const std::string &test_file)
{
  if (!file_exists(a.binary_path))
  {
    std::cerr << "Error: binary not built yet. Compile " << a.display_name << " first.\n";
    return false;
  }
  std::string full_path = a.test_dir + "/" + test_file;
  if (!file_exists(full_path))
  {
    std::cerr << "Error: test file not found: " << full_path << "\n";
    return false;
  }
  std::string cmd = a.binary_path + " " + full_path + " " + a.extra_run_args;
  return std::system(cmd.c_str()) == 0;
}

// Runs the compiled driver on every test file for this assignment.
static bool run_all_tests(const AssignmentEntry &a)
{
  if (!file_exists(a.binary_path))
  {
    std::cerr << "Error: binary not built yet. Compile " << a.display_name << " first.\n";
    return false;
  }
  if (!fs::exists(a.test_dir) || !fs::is_directory(a.test_dir))
  {
    std::cerr << "Error: test directory not found: " << a.test_dir << "\n";
    return false;
  }
  std::string cmd = a.binary_path + " --all " + a.test_dir + " " + a.extra_run_args;
  return std::system(cmd.c_str()) == 0;
}

static void print_assignment_list(const std::vector<AssignmentEntry> &list)
{
  std::cout << "\nAvailable assignments:\n";
  for (size_t i = 0; i < list.size(); ++i)
  {
    std::cout << "  " << (i + 1) << ". " << list[i].display_name << "\n";
  }
}

static int select_assignment(const std::vector<AssignmentEntry> &list)
{
  print_assignment_list(list);
  std::cout << "Select assignment number: ";
  int choice;
  if (!(std::cin >> choice) || choice < 1 || choice > static_cast<int>(list.size()))
  {
    std::cerr << "Error: invalid selection.\n";
    std::cin.clear();
    std::cin.ignore(10000, '\n');
    return -1;
  }
  return choice - 1;
}

static void list_test_files(const AssignmentEntry &a)
{
  std::cout << "\nTest files in " << a.test_dir << ":\n";
  if (!fs::exists(a.test_dir))
  {
    std::cerr << "Error: test directory not found: " << a.test_dir << "\n";
    return;
  }
  std::vector<std::string> files;
  for (const auto &entry : fs::directory_iterator(a.test_dir))
  {
    if (entry.path().extension() == ".txt")
      files.push_back(entry.path().filename().string());
  }
  std::sort(files.begin(), files.end());
  for (const auto &f : files)
    std::cout << "  - " << f << "\n";
}

static void print_menu()
{
  std::cout << "\n================ CS509 Common Wrapper ================\n";
  std::cout << "1. List available assignments\n";
  std::cout << "2. Compile a selected assignment\n";
  std::cout << "3. Run one test file for a selected assignment\n";
  std::cout << "4. Run all test files for a selected assignment\n";
  std::cout << "5. Compile and run ALL submitted assignments (all test files)\n";
  std::cout << "6. Exit\n";
  std::cout << "=======================================================\n";
  std::cout << "Choice: ";
}

int main()
{
  auto assignments = registry();

  while (true)
  {
    print_menu();
    int choice;
    if (!(std::cin >> choice))
    {
      std::cerr << "Error: invalid input.\n";
      std::cin.clear();
      std::cin.ignore(10000, '\n');
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
        std::cout << "Enter test file name (e.g. gemm_test_01.txt): ";
        std::string fname;
        std::cin >> fname;
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
      std::cout << "\nCompiling and running all submitted assignments...\n";
      for (const auto &a : assignments)
      {
        std::cout << "\n---- " << a.display_name << " ----\n";
        if (compile_assignment(a))
        {
          run_all_tests(a);
        }
        else
        {
          std::cerr << "Skipping run for " << a.display_name
                    << " due to compilation failure.\n";
        }
      }
    }
    else if (choice == 6)
    {
      std::cout << "Exiting.\n";
      break;
    }
    else
    {
      std::cerr << "Error: invalid choice. Please select 1-6.\n";
    }
  }

  return 0;
}