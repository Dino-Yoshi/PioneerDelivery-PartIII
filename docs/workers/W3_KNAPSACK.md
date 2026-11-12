# W3 — 0/1 Knapsack Worker Brief

**Worker:** Kiro CLI / Claude Haiku 5.5 Medium, isolated `feature/knapsack` worktree/container. **Tickets:** `B-01`, `B-02`. **Start after G1.**

## Problem and non-negotiable algorithm

Delivery `(id, fuelCost, profit)`; each accepted at most once; total fuel <= budget B, maximize profit. **Use a full two-dimensional bottom-up DP table**. Do not use unbounded knapsack or 1D space reduction as production solution. Target O(nB) time and space.

## State, recurrence, traceback

`K[i][b]` = maximum profit using first i deliveries with total fuel at most b. `K[0][b]=0`, `K[i][0]=0` due to strictly positive costs. If cost_i>b: previous row; otherwise max(previous row without i, `profit_i + K[i-1][b-cost_i]`). Both options **must reference i-1**.

Traceback from `(n,B)`: if `K[i][b]==K[i-1][b]`, exclude on tie; else include and subtract cost then decrement i. Output IDs input order, total fuel used and sum profit; no duplicate IDs, and no hidden minimize-fuel secondary objective.

## Allowed files

`include/pioneer/Knapsack.hpp`, `src/Knapsack.cpp`, `tests/KnapsackTests.cpp`, related fixtures. Request shared-interface changes through Sonnet; no CMake/CLI modifications.

## Required tests

Official P..T, B=10 maximum 53 (Q,R,S is valid); B=0; empty delivery set; item fits exactly; all unaffordable; tied optima; unused budget; greedy ratio counterexample P(6,30), Q(5,24), R(5,24), B=10 optimum 48. Validate 2D size overflow/resource refusal and profit overflow. Independent W5 oracle enumerates all subsets of small n.

## Deliverable

Readable DP and reconstruction with tests, actual command exits/results, memory calculation rationale, scope diff and checkpoint. **Do not commit/merge/push.**
