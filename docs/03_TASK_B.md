# 03 — Task B: 0/1 Knapsack (25 points)

## Assigned problem

Deliveries have `(id, fuelCost, profit)` with positive integer fuel costs and nonnegative integer profits. Scheduling conflicts do not matter. Choose each delivery **at most once** so total fuel cost is <= integer budget `B` and profit is maximal. Return optimum, selected IDs, total fuel, total profit.

The professor explicitly requires a **bottom-up two-dimensional DP table** supporting reconstruction, even though space-optimized 1D approaches exist.

## DP state

`K[i][b]` = maximum profit obtainable from the **first i deliveries** with total fuel cost **at most b**, for `0<=i<=n`, `0<=b<=B`.

- `K[0][b]=0` for all `b`.
- `K[i][0]=0` for all `i` because all fuel costs are strictly positive.

For delivery i with cost `c_i` and profit `v_i`:

- If `c_i>b`: `K[i][b]=K[i-1][b]`.
- Otherwise `K[i][b]=max(K[i-1][b], v_i + K[i-1][b-c_i])`.

**Plain English:** Either skip item i, or take it once and spend its cost, obtaining the best profit from earlier items with the reduced budget. Both choices use the previous row to prohibit repeat selection.

## Table fill and reconstruction

Fill rows increasing `i`; within each row iterate `b=0..B`. Because every dependency is on row `i-1`, budget traversal within a row does not change correctness, but choose increasing b for readability.

Backtrack from `(n,B)`:

1. If `K[i][b]==K[i-1][b]`, exclude i; move to `i-1` (ties exclude).
2. Otherwise include i, subtract `c_i` from b, and move to `i-1`.
3. Stop at i=0; reverse selected IDs so they follow original input order.
4. Recalculate total cost/profit; assert sum(cost)<=B, sum(profit)==K[n][B], no duplicate ID.

When no selection is profitable, empty selection is valid. **Do not introduce a hidden minimize-fuel secondary objective**; equal-profit solutions are acceptable.

## Numeric and memory safeguards

Use signed 64-bit profit accumulators with checked additions; fuel/budget are nonnegative integers of a documented parsed range. Validate `n`, `B`, and `(n+1)*(B+1)` for both arithmetic overflow and a clearly documented safe allocation limit before allocating the 2D table. If too large, return a descriptive resource error; **do not silently switch to a 1D DP table**.

## Complexity

Time **O(nB)**, space **O(nB)**, plus O(n) reconstruction. This is *pseudo-polynomial*: it depends on the **numerical magnitude** of budget B, not merely on the number of bits needed to encode B (`O(log B)`). Explain this distinction in the PDF report.

## Official example (page 2)

```text
5 10
P 4 20
Q 3 14
R 5 29
S 2 10
T 6 31
```

Expected maximum profit **53**; one valid selection `Q,R,S`, fuel 10. Do not require these exact IDs if another optimal solution is returned.

## Adversarial tests

- Empty delivery set, zero budget, exactly fitting item, no affordable item, multiple optimal subsets, unused optimal budget, duplicate IDs rejected, checked overflow and allocation failure.
- Greedy ratio failure: budget 10 with P=(6,30), Q=(5,24), R=(5,24). Ratio greedy takes P=30 but optimal Q+R=48.
- Independently enumerate subsets for small n and verify optimum and invariants. Production code remains 2D DP.

## Definition of done

B-01/B-02 acceptance criteria are met, traces only reference previous rows, official and adversarial tests pass, and the report accurately explains pseudo-polynomial behavior and table dimensions.
