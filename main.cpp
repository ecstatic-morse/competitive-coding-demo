// Problem:
//
// A positive integer, n, is divided by d and the quotient and remainder are q
// and r respectively. In addition d, q, and r are consecutive positive integer
// terms in a geometric sequence, but not necessarily in that order.
//
// For example, 58 divided by 6 has quotient 9 and remainder 4. It can also be
// seen that 4, 6, 9 are consecutive terms in a geometric sequence (common
// ratio 3/2).  We will call such numbers, n, progressive.
//
// Some progressive numbers, such as 9 and 10404 = 102^2, happen to also be
// perfect squares.  The sum of all progressive perfect squares below one
// hundred thousand is 124657.
//
// Find the sum of all progressive perfect squares below one trillion (10^12).

#include <bitset>
#include <cmath>
#include <iostream>
#include <numeric>
#include <unordered_set>

using i64 = int64_t;

// Returns a bitset containing the quadratic residues of the natural numbers mod n.
//
// This requires only O(n) operations to compute because a² mod n = (a mod n)² mod n
//
// A number whose entry is not set in the resulting bitset (mod n) cannot be a
// perfect square because the set contains the square of every natural number
// (mod n).
//
// When the number of quadratic residues is low, and we can immediately rule
// out many candidate perfect squares without resulting to trial division or
// floating point math. For example, n=64 results in a bitset with only 3/16 of
// entries set. In general, moduli which are powers of two result in few
// quadratic residues (why?), while allowing for fast table lookups using bit
// masking.
template<size_t n>
std::bitset<n> quadratic_residues_mod() {
    static_assert(n > 0, "0 is not a valid modulus");

    std::bitset<n> residues{};
    for (size_t i = 0; i < n; ++i) {
        residues.set((i*i) % n);
    }

    return residues;
}

// The modulus used for fast perfect square testing. Should be a power of two.
const size_t MODULUS = 64;
static std::bitset<MODULUS> quadratic_residues = quadratic_residues_mod<MODULUS>();

// Returns true if a number is a perfect square.
bool is_perfect_square(i64 n) {
    if (!quadratic_residues[n % MODULUS])
        return false;

    // If our quadratic residue test is inconclusive, fall back to the naive one.
    auto root = static_cast<i64>(sqrt(n));
    return root*root == n;
}

// We can write a given n as n = d*q + r for some divisor d, quotient
// q, and remainder r, all integers.
//
// Since the harmonic relation can occur in any order, restrict the domain of d
// such that d <= q. Therefore, we have the relative ordering r < d <= q.
//
// Since we consider only harmonically related (r, d, q). We have that d = r *
// (a/b) and q = r * (a/b)². However, both q and d must be integers, which
// implies that we only consider cases where r is divisible by b².
// Define a new integral variable c such that:
//      c = r/b²
//
// Substituting:
//      r = cb²
//      d = cab
//      q = ca²
// And using our initial expression for n
//      n = c²a³b + cb²
//
// We have changed variables so that combinations of inputs generate only
// harmonically related divisors, quotients and remainders.
i64 compute_candidate(i64 a, i64 b, i64 c) {
    return c*c*a*a*a*b + c*b*b;
}

int main() {
    std::unordered_set<i64> sols{};

    // Since we wish to consider n up to 10¹², we must check all a up to 10⁴ (the cube root of 10¹²).
    for (i64 a = 1; a < 10'000; ++a) {
        // r < d <= q implies a/b > 1, or equivalently b < a.
        for (i64 b = 1; b < a; ++b) {
            for (i64 c = 1; ; c += 1) {
                auto n = compute_candidate(a, b, c);
                if (n >= 1'000'000'000'000) break;
                if (is_perfect_square(n)) sols.insert(n);
            }
        }
    }

    for (auto sol : sols) {
        std::cout << (i64)sqrt(sol) << std::endl;
    }

    std::cout << std::endl;
    std::cout << std::accumulate(sols.begin(), sols.end(), 0ll) << std::endl;
    return 0;
}
