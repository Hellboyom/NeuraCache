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

NeuraCache supports both traditional LRU and prediction-aware adaptive caching.

The predictor tracks:

- Total accesses
- Recent accesses
- Access frequency
- Access recency

The prediction score is calculated as:

```text
score = 0.6 × frequency + 0.4 × recency
