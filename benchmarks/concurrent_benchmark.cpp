#include "../src/storage/database.h"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using Clock = std::chrono::high_resolution_clock;

void runBenchmark(int threadCount)
{
  Database database;

  const int operationsPerThread = 25000;

  std::vector<std::thread> threads;

  auto start = Clock::now();

  for (int threadId = 0;
       threadId < threadCount;
       ++threadId)
  {
    threads.emplace_back(
        [&database, threadId]()
        {
          const int operations =
              25000;

          for (int i = 0;
               i < operations;
               ++i)
          {
            std::string key =
                "thread" +
                std::to_string(threadId) +
                "_key" +
                std::to_string(i % 1000);

            if (i % 2 == 0)
            {
              database.set(
                  key,
                  "value");
            }
            else
            {
              std::string value;

              database.get(
                  key,
                  value);
            }
          }
        });
  }

  for (std::thread &thread : threads)
  {
    thread.join();
  }

  auto end = Clock::now();

  double milliseconds =
      std::chrono::duration<double, std::milli>(
          end - start)
          .count();

  long long totalOperations =
      static_cast<long long>(
          threadCount) *
      operationsPerThread;

  double seconds =
      milliseconds / 1000.0;

  double throughput =
      totalOperations /
      seconds;

  std::cout
      << "Threads: "
      << threadCount
      << "\n";

  std::cout
      << "  Operations: "
      << totalOperations
      << "\n";

  std::cout
      << "  Time: "
      << milliseconds
      << " ms\n";

  std::cout
      << "  Throughput: "
      << throughput
      << " ops/sec\n";

  std::cout
      << "------------------------------------\n";
}

int main()
{
  std::cout
      << "====================================\n"
      << "   NeuraCache Concurrent Benchmark\n"
      << "====================================\n\n";

  runBenchmark(1);
  runBenchmark(2);
  runBenchmark(4);
  runBenchmark(8);
  runBenchmark(16);

  std::cout
      << "\nConcurrent benchmark complete.\n";

  return 0;
}