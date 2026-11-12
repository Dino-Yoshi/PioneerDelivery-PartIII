# 04 — Task C: Event Log Cleanup / Subsequence DP (20 points)

## Assigned problem

Given nonempty lowercase strings `source` and `pattern` (pattern initially a subsequence of source), and a sorted array of distinct valid **original, zero-based** positions `targetIndices`, delete the largest possible number of eligible characters while leaving pattern as a subsequence. Deletions **never renumber** input positions. Return maximum deletion count and one optimal ascending list of deleted original indices.

This is **not merely checking subsequence membership**, nor binary-searching a removal prefix. It is the source/pattern alignment DP specified in C1-C3.

## State and initialization

Let `n=|source|`, `m=|pattern|`. Let `eligible[i-1]=1` if original source index `i-1` is in targetIndices, else 0.

`F[i][j]` = **maximum allowed deletions** after examining the first i source characters while using exactly j of those characters, in order, to match the first j pattern characters. All other examined source characters are skipped; an eligible skipped character may be deleted, whereas an ineligible skipped character must remain.

- `F[0][0]=0`.
- `F[0][j>0] = unreachable` (use an explicit safe sentinel such as `-1`, since valid scores are nonnegative; never add to unreachable).
- `F[i][0]` = number of eligible positions in source prefix `[0,i)`: none need to match pattern yet.

For `i>=1`, `0<=j<=m`:

- **Skip source[i-1]**: if `F[i-1][j]` reachable, candidate `F[i-1][j] + eligible[i-1]`. Skipped ineligible characters remain physically present and earn zero.
- **Match source[i-1]**: only when `j>=1`, `source[i-1]==pattern[j-1]`, and `F[i-1][j-1]` reachable. Candidate `F[i-1][j-1]`, with no deletion credit.
- `F[i][j]` is the maximum among valid candidates; if none valid, it remains unreachable.

Fill i from 1 through n and j from 0 through m. Final optimum `F[n][m]` is reachable under the PDF's assumption.

**Why this works:** Every source character is either assigned to the chosen subsequence alignment, or skipped. It is always optimal to delete an eligible skipped character because deletion doesn't affect the chosen matched positions. Ineligible skipped characters stay but don't prevent pattern remaining a subsequence.

## Reconstruction (original indices)

Starting at `(i=n,j=m)`, check whether a valid **skip** transition equals `F[i][j]`. Prefer skip on ties for determinism. On skip, if index i-1 is eligible, record original index `i-1`; decrement i but not j.

Otherwise require a matching transition from `(i-1,j-1)` and decrement both. Stop at `(0,0)`. Reverse collected original indices into ascending order.

Verification independent of the traceback: construct the kept string by removing exactly the recorded original indices (simultaneously, no index shifting), then perform a fresh linear subsequence check of pattern against kept source. Also assert returned length equals optimum, positions belong to targetIndices and are unique/in-bounds. Return 0 and empty indices if no deletions possible.

## Complexity

- Eligible lookup setup `O(n+|targetIndices|)` using a Boolean array.
- 2D DP fill `O(nm)` time, `O(nm)` space.
- Traceback O(n+m); result O(n). Overall `O(nm)` time and `O(nm)` auxiliary space for nonempty n,m, as targeted. If reporting all terms, mention preprocessing separately.
- Guard multiplication of `(n+1)*(m+1)` and allocation before building the table.

## Four official examples (pages 3–4)

| ID | source | pattern | targetIndices | optimum |
|---|---|---|---|---:|
| C-01 | `abbaa` | `aba` | `[0,1,2]` | 1 |
| C-02 | `bcda` | `d` | `[0,3]` | 2 |
| C-03 | `dda` | `dda` | `[0,1,2]` | 0 |
| C-04 | `yeyeykyded` | `yeyyd` | `[0,2,3,4]` | 2 |

For C-01, either deletion 1 or 2 is valid. For C-04, deletion indices `[2,3]` are one valid optimum. The PDF has a stray copied paragraph after C-04; do not treat it as a different requirement.

## Counterexample to earliest-match greed

`source="abaaba"`, `pattern="aba"`, `targetIndices=[0,1,2]` has optimum **3** by deleting the first three characters and matching the last `aba`. Greedily aligning to the first three source characters would prevent those removals.

## Other tests

No eligible positions; eligible positions all removable; repeated characters with multiple alignments; pattern identical to source; eligible index 0 and last index; indices sorted/distinct validation; source that initially lacks pattern (reject). Independent brute-force subset enumeration **only in small test oracle**, followed by kept-string validation.

## Definition of done

C-01/C-02 tickets meet the 2D DP and reconstruction requirements, verify against all four official examples, and pass adversarial oracle/property tests; source indices remain original and stable.
