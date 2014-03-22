

#if ! defined ( Clavier_H )
#define MAIN_H


int is_prime(uint64_t p);
void find_prime_factors(uint64_t n);
void print_prime_factors(uint64_t n);
void readNumber(FILE *f);
int get_prime_factors(uint64_t n, uint64_t* dest);

#endif // MAIN_H
