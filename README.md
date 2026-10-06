# NeuraCache

**NeuraCache** is a high-performance, adaptive in-memory database and caching system built in **C++17**.

It combines traditional LRU caching with access-pattern prediction to make smarter eviction decisions. The project also includes persistence, a RESP-inspired TCP protocol, runtime metrics, benchmarking, an experimental machine-learning predictor, and a real-time React monitoring dashboard.

---

## Overview

Traditional LRU caching assumes that recently accessed data is the data most likely to be accessed again.

NeuraCache extends this idea by tracking both **frequency and recency of access** and using a lightweight predictor to estimate which keys are likely to remain useful.

When the cache reaches capacity, NeuraCache uses these predictions to make eviction decisions instead of relying only on recency.

The project also includes an experimental ML pipeline that evaluates whether a learned model can improve access prediction over the built-in heuristic approach.

---

## Key Features

### Core Database

- In-memory key-value storage
- `SET`
- `GET`
- `DEL`
- `EXISTS`
- `EXPIRE`
- `TTL`
- `INCR`
- `DECR`
- `DBSIZE`
- `FLUSHDB`
- `INFO`

### Networking

- TCP server
- Default port: `6379`
- Persistent client connections
- Multiple concurrent clients
- Request pipelining
- Fragmented request handling
- Partial response handling
- Graceful shutdown

### Adaptive Caching

NeuraCache supports prediction-aware adaptive caching built on top of an LRU cache.

The predictor tracks:

- Total accesses
- Recent accesses
- Access frequency
- Access recency

The prediction score is calculated as:

```text
score = 0.6 × frequency + 0.4 × recency


                     ┌─────────────────────┐
                     │   React Dashboard   │
                     │                     │
                     │ Metrics / Charts    │
                     │ AI Predictions      │
                     └──────────┬──────────┘
                                │ HTTP
                                ▼
                     ┌─────────────────────┐
                     │ Node.js API Bridge  │
                     │       :3001         │
                     └──────────┬──────────┘
                                │ RESP
                                ▼
┌──────────────────────────────────────────────────────┐
│                 NeuraCache Server :6379              │
│                                                      │
│  ┌──────────────┐      ┌──────────────────────────┐ │
│  │ RESP Parser  │─────▶│ Command Handler          │ │
│  └──────────────┘      └────────────┬─────────────┘ │
│                                     │               │
│                                     ▼               │
│                           ┌─────────────────────┐   │
│                           │ Database            │   │
│                           │                     │   │
│                           │ Key / Value Storage │   │
│                           └─────────┬───────────┘   │
│                                     │               │
│                           ┌─────────▼───────────┐   │
│                           │ Adaptive Cache      │   │
│                           └─────────┬───────────┘   │
│                                     │               │
│                    ┌────────────────┴────────────┐  │
│                    ▼                             ▼  │
│              ┌───────────┐                 ┌───────────┐
│              │ LRU Cache │                 │ Predictor │
│              └───────────┘                 └───────────┘
│                                                      │
│              ┌───────────┐      ┌─────────────────┐ │
│              │  Metrics  │      │   Persistence   │ │
│              └───────────┘      └─────────────────┘ │
└──────────────────────────────────────────────────────┘



AI Prediction
The production predictor is intentionally lightweight and does not require a machine-learning runtime.
For each tracked key, NeuraCache maintains:
- totalAccesses
- recentAccesses
The predictor combines normalized frequency and recency:
prediction score = 0.6 × frequency + 0.4 × recency
The system exposes prediction information through:
- PREDICT
- TOPPREDICT
- ANALYZE
Example:
AI CACHE ANALYSIS
total_accesses: 10
tracked_keys: 1
top_keys:
  count score=1.000000 accesses=10

Machine Learning Experiment
The project includes a separate ML experiment to evaluate whether a learned model can improve future access prediction.
The experiment uses:
- Synthetic cache access data
- Logistic Regression
- A dummy baseline
- The built-in heuristic predictor
Results
Predictor	Accuracy
Baseline	66.75%
Heuristic	67.85%
Logistic Regression	74.00%


The ML model achieved a 7.25 percentage-point improvement over the baseline and a 6.15 percentage-point improvement over the heuristic predictor on the synthetic evaluation dataset.
These results are based on controlled synthetic data and should not be interpreted as production workload accuracy.

The ML experiment is intentionally kept separate from the production C++ predictor so that the core system remains lightweight and dependency-free.
Performance
Representative benchmark results from the development machine:
Workload	Throughput
SET	~250K ops/sec
GET	~1.8M ops/sec
Mixed workload	~430K ops/sec


Adaptive Cache Benchmark
Under a hostile synthetic scan workload:
Cache Policy	Hit Rate
Traditional LRU	0%
Adaptive Cache	33.3%


The adaptive policy reduced misses from 1500 to 1000 in this benchmark.
This demonstrates the potential advantage of prediction-aware eviction for workloads where traditional LRU performs poorly. It is not intended to claim universal superiority over LRU.
Persistence
NeuraCache supports binary snapshot persistence.
Snapshots store:
- Keys
- Values
- Remaining TTL information
The snapshot lifecycle is:
Server Startup
      │
      ▼
Load Snapshot
      │
      ▼
Restore Database

and:
Server Shutdown
      │
      ▼
Write Snapshot

Backup snapshot handling is also supported.
Runtime Metrics
NeuraCache tracks:
- Total commands
- GET commands
- SET commands
- DELETE commands
- Cache hits
- Cache misses
- Evictions
- Expired keys
- Uptime
The INFO command exposes these statistics.
The monitoring dashboard uses them to calculate real-time cache performance metrics such as hit rate.
Monitoring Dashboard
The project includes a React + Vite monitoring dashboard.
The dashboard provides:
- Server status
- Runtime statistics
- Cache hit rate
- GET / SET / DELETE activity
- Cache evictions
- Expired keys
- Live command activity
- AI predictions
- Prediction scores
- AI tracked keys
- AI access statistics
- Runtime uptime
Dashboard Architecture
React Dashboard
       │
       │ HTTP
       ▼
Node.js API Bridge
       │
       │ TCP / RESP
       ▼
NeuraCache

This keeps the monitoring interface separate from the core C++ database implementation.
Project Structure
NeuraCache/
│
├── benchmarks/
│   ├── benchmark.cpp
│   ├── concurrent_benchmark.cpp
│   └── adaptive_benchmark.cpp
│
├── docs/
│   └── architecture.md
│
├── frontend/
│   ├── server/
│   │   └── index.cjs
│   └── src/
│       ├── App.jsx
│       ├── App.css
│       └── index.css
│
├── ml/
│   ├── generate_dataset.py
│   ├── train_predictor.py
│   └── evaluate_predictors.py
│
├── src/
│   ├── ai/
│   │   ├── predictor.cpp
│   │   └── predictor.h
│   │
│   ├── commands/
│   │   ├── command_handler.cpp
│   │   └── command_handler.h
│   │
│   ├── eviction/
│   │   ├── lru_cache.cpp
│   │   ├── lru_cache.h
│   │   ├── adaptive_cache.cpp
│   │   └── adaptive_cache.h
│   │
│   ├── metrics/
│   │   ├── metrics.cpp
│   │   └── metrics.h
│   │
│   ├── persistence/
│   │   ├── snapshot.cpp
│   │   └── snapshot.h
│   │
│   ├── protocol/
│   │   ├── resp.cpp
│   │   └── resp.h
│   │
│   ├── server/
│   │   ├── server.cpp
│   │   └── server.h
│   │
│   └── storage/
│       ├── database.cpp
│       └── database.h
│
├── tests/
│   ├── test_commands.cpp
│   ├── test_database.cpp
│   ├── test_resp.cpp
│   ├── test_server.cpp
│   ├── test_lru.cpp
│   ├── test_concurrency.cpp
│   ├── test_snapshot.cpp
│   ├── test_metrics.cpp
│   ├── test_adaptive_cache.cpp
│   └── test_adaptive_integration.cpp
│
├── CMakeLists.txt
└── README.md

Getting Started
Requirements
- C++17 compatible compiler
- CMake
- macOS or Linux
- Node.js and npm
- Python 3 for the ML experiment
Build NeuraCache
cmake -S . -B build
cmake --build build -j

Run the Server
./build/neuracache

The server starts on:
127.0.0.1:6379

You can connect using:
nc 127.0.0.1 6379

Run the Dashboard
In another terminal:
cd frontend
npm install
node server/index.cjs

Then start the React development server:
npm run dev

Open:
http://localhost:5173
The API bridge runs on:
http://localhost:3001
Testing
Build the project:
cmake -S . -B build
cmake --build build

Run the test suite:
cd build
ctest --output-on-failure

The tests cover:
- Database operations
- Command handling
- RESP parsing
- LRU behavior
- Adaptive cache behavior
- Adaptive cache integration
- Concurrency
- Persistence
- Runtime metrics
ML Experiment Setup
Create a Python environment:
python3 -m venv ml/.venv
source ml/.venv/bin/activate

Install dependencies:
python -m pip install numpy scikit-learn

Generate the dataset:
python ml/generate_dataset.py

Train the predictor:
python ml/train_predictor.py

Evaluate the predictors:
python ml/evaluate_predictors.py

Design Decisions
Why C++?
C++ provides:
- Low-level memory control
- High performance
- Efficient data structures
- Direct socket programming
- Strong concurrency primitives
Why Adaptive Eviction?
LRU works well when recent accesses are strong indicators of future accesses.
However, scan-heavy workloads can cause frequently reused keys to be repeatedly evicted.
NeuraCache supplements recency with access-pattern prediction to make eviction decisions based on estimated future usefulness.
Why Keep ML Separate?
The ML experiment demonstrates the feasibility of learned prediction without introducing Python or model-runtime dependencies into the production cache.
This keeps the core system:
- Lightweight
- Fast
- Deterministic
- Easy to build
while providing a foundation for future learned caching strategies.
Current Limitations
NeuraCache is not intended to be a complete Redis replacement.
Current limitations include:
- Single-node architecture
- Thread-per-client networking model
- In-memory primary storage
- Experimental ML predictor
- No distributed clustering
- No authentication
- No AOF persistence
- Limited Redis command compatibility
These are deliberate scope boundaries for the current version.
Future Work
Potential future improvements include:
- More advanced learned cache policies
- Training on real-world access traces
- Sharded storage
- Connection pooling
- More scalable concurrency architecture
- Additional Redis-compatible commands
- AOF-style durability
- Distributed caching
- More sophisticated workload-aware benchmarking
Engineering Focus
NeuraCache was built to explore practical systems concepts including:
Systems Programming + Data Structures + Caching Algorithms + Concurrency + Networking + Persistence + Performance Engineering + Machine Learning + Observability
The goal is not simply to implement a key-value store, but to explore how adaptive algorithms and workload prediction can influence systems-level decisions.
License
This project is intended as a software engineering and systems project for educational and portfolio purposes.


