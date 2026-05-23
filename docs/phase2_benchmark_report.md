# Phase 2 Benchmark Report

## Setup

- Executable: `video_labs_phase2`
- Producer count: 1
- Consumer counts: 1, 2, 4, 8
- Item counts: 100,000 and 1,000,000
- Consumer work: 100 simulated work iterations per item
- Burst size: 1
- Producer delay: 0 microseconds
- Build: normal debug build via `make run-phase2`

## Commands

```bash
make run-phase2 ITEMS=100000 WORK=100 CONSUMERS=1 CSV=phase2_results.csv
make run-phase2 ITEMS=100000 WORK=100 CONSUMERS=2 CSV=phase2_results.csv
make run-phase2 ITEMS=100000 WORK=100 CONSUMERS=4 CSV=phase2_results.csv
make run-phase2 ITEMS=100000 WORK=100 CONSUMERS=8 CSV=phase2_results.csv

make run-phase2 ITEMS=1000000 WORK=100 CONSUMERS=1 CSV=phase2_results.csv
make run-phase2 ITEMS=1000000 WORK=100 CONSUMERS=2 CSV=phase2_results.csv
make run-phase2 ITEMS=1000000 WORK=100 CONSUMERS=4 CSV=phase2_results.csv
make run-phase2 ITEMS=1000000 WORK=100 CONSUMERS=8 CSV=phase2_results.csv
```

## Results

| Items | Consumers | Elapsed Seconds | Throughput Items/Sec | Speedup vs 1 Consumer |
| ----- | --------- | --------------- | -------------------- | --------------------- |
| 100,000 | 1 | 0.0919971 | 1,086,990 | 1.00x |
| 100,000 | 2 | 0.0609279 | 1,641,280 | 1.51x |
| 100,000 | 4 | 0.0588518 | 1,699,180 | 1.56x |
| 100,000 | 8 | 0.119878 | 834,181 | 0.77x |
| 1,000,000 | 1 | 0.809437 | 1,235,430 | 1.00x |
| 1,000,000 | 2 | 0.64852 | 1,541,970 | 1.25x |
| 1,000,000 | 4 | 0.565661 | 1,767,840 | 1.43x |
| 1,000,000 | 8 | 1.36696 | 731,551 | 0.59x |

## Observations

- Four consumers produced the best throughput for both tested item counts.
- Two consumers improved throughput over one consumer, but scaling was sublinear.
- Eight consumers were slower than one consumer in both runs.
- The checksum and work checksum stayed consistent across consumer counts, which validates that all items were processed exactly once.
- The drop at eight consumers suggests lock contention and thread scheduling overhead dominate the available parallelism for this workload.

## Next Steps

- Add repeated runs and aggregate mean/min/max throughput to reduce variance.
- Add latency measurements for per-item processing time.
- Compare debug and release builds before treating the absolute throughput numbers as final.
