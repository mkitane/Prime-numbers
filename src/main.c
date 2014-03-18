#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "main.h"

int is_prime(uint64_t p)
{
    if(p==2){
        return 1; 
    }
    //Test par 2
    if(p%2==0){
        return 0;
    }

    uint64_t i;
    //Si echoue on verifie qu'avec les nombres impaires
    for(i=3 ; i<(int)sqrt(p) ; i+=2){
        if(p%i==0){
            return 0;
        }
    }
    return 1; //si premier

}

void find_prime_factors(uint64_t n)
{
    uint64_t i; 

    if(is_prime(n) == 1){
        printf("%llu \n",n);
        return;
    }
    
    for(i=2; i< n ; i++){
        if(n%i == 0 && is_prime(i) == 1){
            printf("%llu ", i);
            find_prime_factors((uint64_t)n/i);
            return;
        }
    }

}


void print_prime_factors(uint64_t n)
{
    printf("%llu : ",n);
    find_prime_factors(n);
}


void open_file_and_find_prime_factors()
{
    FILE *f = fopen("numbers.txt", "r");
 
    uint64_t number;
    
    while(fscanf(f, "%llu",&number) != EOF) {
        print_prime_factors(number);
    }
	fclose(f);
}
int main()
{
    
//    uint64_t i = 92;
//    if(is_prime(i) == 1){
//        printf("Le nombre %llu est premier\n",i);
//    }else{
//        printf("Le nombre %llu n'est pas premier \n",i);
//    }
    
//    print_prime_factors(84);
    
    open_file_and_find_prime_factors();
    
}
