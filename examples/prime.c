#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stc/cbits.h>

cbits sieveOfEratosthenes(size_t n)
{
    cbits bits = cbits_with_size(n>>1, true);
    size_t q = (size_t) sqrt(n);

    for (size_t i = 3; i <= q; i += 2) {
        for (size_t j = i; j < n; j += 2) {
            if (cbits_test(bits, j>>1)) {
                i = j;
                break;
            }
        }
        for (size_t j = i*i; j < n; j += i*2)
            cbits_reset(&bits, j>>1);
    }
    return bits;
}

int main(void)
{
    size_t n = 100000000;
    printf("computing prime numbers up to %zu\n", n);

    clock_t t1 = clock();
    cbits primes = sieveOfEratosthenes(n + 1);
    size_t np = cbits_count(primes);
    clock_t t2 = clock();

    printf("number of primes: %zu, time: %f\n", np, (t2 - t1) / (float)CLOCKS_PER_SEC);

    printf(" 2"); for (size_t i = 3; i < 1000; i += 2)
       if (cbits_test(primes, i>>1)) printf(" %zu", i);
    puts("");

    cbits_del(&primes);
}
