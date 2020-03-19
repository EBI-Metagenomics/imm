# Compact dynamic programming structures

## Context: my CPU

- **Model**: Intel(R) Core(TM) i7-7567U CPU @ 3.50GHz
- **L1**: 128 KiB (L1D: 2x32 KiB, L1I: 2x32 KiB)
- **L2**: 512 KiB (2x256 KiB)
- **L3**: 4 MiB (2x2 MiB)
- **L4**: 64 MiB

## Structures

- M: number of states
- L: sequence length

```c
char alphabet = "AGT";
unsigned emission_lenghts[] = {1, 2, 3};
```

Implied ahead-code table:

- A  : 00, G  : 01, T  : 02,
- AA : 03, AG : 04, AT : 05,
- GA : 06, GG : 07, GT : 08,
- TA : 09, TG : 10, TT : 11,
- AAA: 12, AAG: 13, AAT: 14,
- AGA: 15, AGG: 16, AGT: 17,
- ATA: 18, ATG: 19, ATT: 20,
- GAA: 21, GAG: 22, GAT: 23,
- GGA: 24, GGG: 25, GGT: 26,
- GTA: 27, GTG: 28, GTT: 29,
- TAA: 30, TAG: 31, TAT: 32,
- TGA: 33, TGG: 34, TGT: 35,
- TTA: 36, TTG: 37, TTT: 38,

Let `AC(seq)` be the ahead-code for sequence `seq`.

```c
char sequence[L] = "AGGT;
unsigned ahead_sequence[] = {AC("A"), AC("AG"), AC("AGG"), /* first sequence position  */
                             AC("G"), AC("GG"), AC("GGT"), /* second sequence position */
                             AC("G"), AC("GT"),            /* third sequence position  */
                             AC("T")};                     /* fourth sequence position */
```

We will use `ahead_sequence` in place of `sequence`.

States will be the numbers 0, 1, ..., M-1. Let `T(i, qi(j))`  be the transition
cost from state `i` to state `qi(j)`. Let `NT(i)` be the number of transitions
of a given state `i`. We will lay out the transition array as follows:

```c
struct transition
{
  double cost = {
    T(0, q0(0)), T(0, q0(1)), ..., T(0, q0(NT(0)-1)), /* transitions from state 0 */
    T(1, q1(0)), T(1, q1(1)), ..., T(1, q1(NT(1)-1)), /* transitions from state 1 */
    ...,
  };
  unsigned offset = {0, NT(0), NT(0) + NT(1), NT(0) + NT(1) + ... + NT(M-1)};
};
```

```c
struct emission
{
  double cost[] = {
    EC(0, AC("A")), EC(0, AC("G")), , ..., EC(0, AC("TTT")),   /* emissions for state 0 */
    EC(1, AC("AAA")), EC(1, AC("AAG")), ..., EC(1, AC("TTT")), /* emissions for state 1 */
    ...,
  };
  unsigned offset = {0, NE(0), NE(0) + NE(1), ..., NE(0) + NE(1) + ... + NE(M-1)};
};
```

Let `F(i, j)` be the `ahead_sequence` index regarding sequence position `i` and
emission length `emission_lenghts[j]`.

```c
struct dp_matrix
{
  double [i][]
};
```
