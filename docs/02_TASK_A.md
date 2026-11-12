# 02 — Task A: Weighted Interval Scheduling (35 points)

## Assigned problem

Given deliveries `(id, start, finish, profit)` on one driver, choose a pairwise compatible subset with maximum sum of profits. `start < finish`, `profit >= 0`; a finish at time `t` is compatible with a next start at `t`. Return maximum profit and selected IDs in chronological order. Expected O(n log n) total time and O(n) DP/predecessor storage (sorting may have separate storage).

## Approved representation

- CLI time coordinates: **signed 64-bit integers** (documented elective restriction).
- Profit text: nonnegative exact decimal with 0, 1, or 2 fractional digits; parse to **signed 64-bit cents** without floating-point conversion.
- Result formatting converts cents back into exactly two decimal digits.
- Explicitly reject invalid input and arithmetic overflow.

## Preprocessing and stable ordering

1. Retain each delivery's original ID and original ordinal.
2. Sort by **nondecreasing finish time**, with a deterministic secondary order (e.g., start then original ordinal) that does not change correctness.
3. Use 1-based mathematical indices for analysis: sorted deliveries `1..n`, with `DP[0]=0`.
4. `p(i) = max { j : 1 <= j < i and finish[j] <= start[i] }`, otherwise `0`.
5. Search **only indices before i**, using binary search over sorted finishes, for O(log n) per predecessor. `finish == start` is compatible. Do not accidentally search the current delivery or later indices.

## DP recurrence

Let `w_i` be delivery i's profit in cents.

- `DP[0] = 0`.
- `DP[i] = max(DP[i-1], w_i + DP[p(i)])` for `i = 1..n`.

**Plain English:** For every delivery in finish order, either skip it and retain the best prior profit, or take it and add its profit to the best schedule ending before it starts. Keep the better total.

Both candidates are valid by inductive optimal substructure. Any optimum either contains i or does not. In the former case, earlier chosen jobs lie in prefix `p(i)`; replacing them with a better prefix would improve the optimum, a contradiction.

## Reconstruction

Starting with `i=n`:

1. If `DP[i] == DP[i-1]`, **exclude i** (deterministic ties), decrement `i`.
2. Otherwise include sorted delivery i and set `i=p(i)`.
3. Continue until `i=0`; reverse recorded IDs into chronological finish-time order.
4. Recompute total cents and assert every neighboring selected interval satisfies `finish(prev) <= start(next)`.

For empty input return profit `0.00` and no IDs. Zero-profit jobs may be omitted because ties favor exclusion. Any optimal schedule is acceptable even if the PDF gives a different optimal subset.

## Complexity accounting

- Sorting `O(n log n)` comparisons (auxiliary storage depends on chosen sorting implementation).
- Predecessor array with binary search `O(n log n)` time, `O(n)` storage.
- One DP pass `O(n)` time and storage.
- Traceback `O(n)` time in the worst case, `O(n)` selected-ID storage.
- Total: **O(n log n) time, O(n) auxiliary algorithm storage**, plus sorting-specific storage.

## Official example (page 1)

```text
6
A 0 3 50.00
B 1 4 60.00
C 3 5 40.00
D 4 7 70.00
E 5 8 50.00
F 7 9 50.00
```

Expected optimum `180.00`; one valid chronological selection: `B,D,F`. Greedy earliest finish can return `A,C,E` for profit `140.00` (it maximizes count, not profit).

## Edge and adversarial cases

- `n=0`; all jobs overlapping; endpoint-touching; duplicate finish times; zero profits; ties; negative time values with positive-duration intervals; input ordering shuffled; overflow boundaries.
- Counterexample: `(A,0,2,10)`, `(B,0,3,100)`, `(C,2,4,10)` gives optimum B=100 versus greedy A+C=20.
- Use an **independent exhaustive subset oracle only when n is small**. Check profit, compatibility, unique IDs, and chronological order.

## Definition of done

The A-01/A-02 tickets pass, computed `p(i)` and recurrence are reviewable, official and adversarial tests pass, traceback is valid under ties, and complexity/documentation match the actual implementation.
