# advent2021-fast

**These solutions are a work in progress.**

[Advent of Code 2021](https://adventofcode.com/2021/) optimized C++ solutions.

Here are the timings from an example run on an i9-9980HK CPU laptop.  Times will vary depending on inputs.

    Day 01      44 μs
    Day 02       4 μs
    Day 03       8 μs
    Day 04      14 μs
    Day 05      53 μs
    Day 06       2 μs
    Day 07       9 μs
    Day 08      28 μs
    Day 09      68 μs
    Day 10      19 μs
    Day 11      11 μs
    Day 12      20 μs
    Day 13      28 μs
    Day 14       5 μs
    Day 15    4047 μs
    Day 16       8 μs
    Day 17       1 μs
    Day 18    1344 μs
    Day 19    1220 μs
    Day 20     304 μs
    Day 21     310 μs
    Day 22     742 μs
    Day 23     277 μs
    Day 24       2 μs
    Day 25     393 μs
    -----------------
    Total:    8961 μs

Solutions should work with any puzzle input, provided it is byte-for-byte an exact copy of the file downloaded from Advent of Code.

# Summary of solutions

Here are a few brief notes about each solution.

## Day 1

The input numbers are small enough to fit in a 16-bit integer.  Because we need to examine only a small sliding window, we can pack the four most recently seen values into a 64-bit integer and use bit shifting to advance the window.

    // Using bit shift
    previous = (previous << 16) + n;

    // Equivalent version
    previous[3] = previous[2]
    previous[2] = previous[1]
    previous[1] = previous[0]
    previous[0] = n

## Day 2

To parse the commands `down` `up` and `forward`, we only need to test if the first letter is `d` `u` or `f`.

## Day 3

Each of the 12-bit input numbers is unique, so they can be sorted by creating and iterating over a 4096-element bitset.  Part 2 uses binary search to find the points where each bit changes from 0 to 1, then recursively searches either the 0's or 1's depending on which is more frequent.

## Day 4

This solution creates a linked list of bingo boards and grid locations for each of the draw numbers.  Marked boards are stored as 25 bits in an integer, and testing for a win is just two bit-mask and compare operations: one each for the marked row and column.

## Day 5

After trying several different approaches, the one that seemed to work best was to scan the coordinate space row-by-row from top to bottom.  Active line segments are tracked separately for the horizontal, vertical, diagonal, and anti-diagonal cases.  For each of these cases, there is an array of counts (how many active line segments per vertical/diagonal/etc coordinate), a bitmap of which cells intersect at least one line, and a bitmap of which cells intersect multiple lines along the same vertical/diagonal/etc.

Handling each line orientation separately simplifies advancing to the next row (bitwise shift left/right for the diagonal orientations.)  Parts 1 and 2 can also be solved at the same time.

## Day 6

The total lanternfish population satisfies the recurrence `f(n) = f(n - 7) + f(n - 9)`.  After setting up initial conditions (populations for first 9 days), it just iterates out the remaining days.

## Day 7

The optimal position for Part 1 is the median, and for Part 2 it is either the floor or ceiling of the mean.  All computation is done using prefix-sum arrays (indexed by x-coordinate) for the first three powers of the x-coordinate (`x^0`, `x^1`, `x^2`).  This avoids having to sort the list of crab submarines.

## Day 8

Digit strings are converted to 7-bit numbers representing which segments are lit.  The bitwise XOR and AND of the "easy" (Part 1) digit segments, together with the segment counts (5 or 6), uniquely identify the Part 2 digits.

## Day 9

Both parts are solved in a single scan over the input.  Contiguous intervals of basin locations (digits 0-8) are joined with neighboring intervals from the previous row using the union-find algorithm.  Basin size and low point are propagated up to the disjoint set root nodes.

## Day 10

Relatively straightforward solution to the parenthesis matching problem.  Uses `std::nth_element` to find the middle score without fully sorting the list.

## Day 11

## Day 12

## Day 13

## Day 14

## Day 15

## Day 16

## Day 17

## Day 18

## Day 19

## Day 20

## Day 21

## Day 22

## Day 23

## Day 24

## Day 25
