#include "../src/storage/database.h"
#include "../src/metrics/metrics.h"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using Clock = std::chrono::high_resolution_clock;

void printResult(
    const std::string &name,
    long long operations,
    double milliseconds)
{
  double seconds = milliseconds / 1000.0;

  double throughput =
      operations / seconds;

  double latency =
      milliseconds * 1000.0 / operations;

  std::cout
      << "\n"
      << name
      << "\n"
      << "  Operations: "
      << operations
      << "\n"
      << "  Time: "
      << milliseconds
      << " ms\n"
      << "  Throughput: "
      << throughput
      << " ops/sec\n"
      << "  Average latency: "
      << latency
      << " us/op\n";
}

void benchmarkSet()
{
  Database database;

  const int operations = 100000;

  auto start = Clock::now();

  for (int i = 0;
       i < operations;
       ++i)
  {
    database.set(
        "key" + std::to_string(i),
        "value" + std::to_string(i));
  }

  auto end = Clock::now();

  double milliseconds =
      std::chrono::duration<double, std::milli>(
          end - start)
          .count();

  printResult(
      "SET benchmark",
      operations,
      milliseconds);
}

void benchmarkGet()
{
  Database database;

  const int operations = 100000;

  for (int i = 0;
       i < operations;
       ++i)
  {
    database.set(
        "key" + std::to_string(i),
        "value");
  }

  std::string value;

  auto start = Clock::now();

  for (int i = 0;
       i < operations;
       ++i)
  {
    database.get(
        "key" + std::to_string(i),
        value);
  }

  auto end = Clock::now();

  double milliseconds =
      std::chrono::duration<double, std::milli>(
          end - start)
          .count();

  printResult(
      "GET benchmark",
      operations,
      milliseconds);
}

void benchmarkMixed()
{
  Database database;

  const int operations = 100000;

  for (int i = 0;
       i < 50000;
       ++i)
  {
    database.set(
        "key" + std::to_string(i),
        "value");
  }

  std::string value;

  auto start = Clock::now();

  for (int i = 0;
       i < operations;
       ++i)
  {
    if (i % 2 == 0)
    {
      database.set(
          "key" + std::to_string(i % 50000),
          "value");
    }
    else
    {
      database.get(
          "key" + std::to_string(i % 50000),
          value);
    }
  }

  auto end = Clock::now();

  double milliseconds =
      std::chrono::duration<double, std::milli>(
          end - start)
          .count();

  printResult(
      "Mixed SET/GET benchmark",
      operations,
      milliseconds);
}

int main()
{
  std::cout
      << "====================================\n"
      << "        NeuraCache Benchmark\n"
      << "====================================\n";

  benchmarkSet();

  benchmarkGet();

  benchmarkMixed();

  std::cout
      << "\nBenchmark complete.\n";

  return 0;
}