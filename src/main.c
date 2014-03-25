#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "main.h"
#include <pthread.h>

//TODO
#define MAX_FACTORS 64
#define MAX_TAB 500000

static pthread_mutex_t lock;
static uint64_t tab[MAX_TAB][MAX_FACTORS];
static pthread_mutex_t lockTab;


//-------------Methods to implement the Hash table
// Hash function
uint64_t hash(uint64_t toHash) {
    // 2 XOR shifts and one multiplication
    toHash ^= toHash >> 23;
    toHash *= 0x2127599bf4325c37ULL;
    toHash ^= toHash >> 47;
    
    // Modulo to restrict results
    toHash %= MAX_TAB;
    //printf("%llu\n", toHash);
    return toHash;
}
int isTheRightNumber(uint64_t number)
{
    
    uint64_t supposedNumber = 1;
    int j = 0;
    uint64_t n = hash(number);
    
    
    pthread_mutex_lock(&lockTab); //Lock file access
    while(tab[n][j] != 0){
        supposedNumber *= tab[n][j];
        j++; 
    }
    pthread_mutex_unlock(&lockTab);            //Unlock file access
    
    
    if(supposedNumber==number){
        return 1;
    }
    return 0;
}
void copyToTab(uint64_t hashn, uint64_t *dest, int nb_fact){
    int i;
    //Si le nombre na pa deja été memorisé, on le fait
    pthread_mutex_lock(&lockTab); //Lock file access
    for(i = 0; i < nb_fact ; i++){
        tab[hashn][i] = dest[i];
    }
    pthread_mutex_unlock(&lockTab);            //Unlock file access
}
int copyToDest(uint64_t index, uint64_t *dest, int nb_fact){
    int j = 0;
     
    pthread_mutex_lock(&lockTab); //Lock file access
    while(tab[index][j] != 0){
        dest[j] = tab[index][j];
        j++;
        nb_fact++;
    }
    pthread_mutex_unlock(&lockTab);            //Unlock file access

    return nb_fact;
}


int is_prime(uint64_t p)
{
    uint64_t racine = (uint64_t) sqrt(p);

    if(p==2 || p==3 || p==5){
        return 1;
    }
    //Test par 2
    if(p%2==0 || p%3==0 || p%5==0){
        return 0;
    }
    
    uint64_t i;
    //Si echoue on verifie qu'avec les nombres impaires
    for(i=11 ; i<racine ; i+=6){
        if(p%(i-4)==0 && is_prime((i-4))){
            return 0;
        }
        if(p%(i)==0 && is_prime((i))){
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
            break;
        }
    }
	find_prime_factors((uint64_t)n/i);
}



//-----Q8-----//
uint64_t find_next_prime_factor(uint64_t n)
{
     uint64_t racine = (uint64_t) sqrt(n);

    //Test par 2
    if(n%2==0){
        return 2;
    }
    
if(n%3==0){
	return 3;
}
if(n%5 == 0){
	return 5;
}
    uint64_t i;
    //Si echoue on verifie qu'avec les nombres impaires
    for(i=11 ; i<racine ; i+=6){
        if(n%(i-4)==0 && is_prime((i-4))){
            return (i-4);
        }
        if(n%(i)==0 && is_prime((i))){
            return i;
        }
    }
    return n; //si premier
    
}

int get_prime_factors(uint64_t n, uint64_t* dest)
{
    uint64_t ndepart = n;
    uint64_t hashn = hash(n);
    
	int nb_fact = 0;
	
    if (n == 0)
    {
        return nb_fact;
    }
    
    
    pthread_mutex_lock(&lockTab); //Lock file access
    uint64_t tabempty = tab[hashn][0];
    pthread_mutex_unlock(&lockTab);            //Unlock file access
    
    
    
    //On recupere le nombre mémorisé si on l'a deja
    if(tabempty != 0){         //cad que l'on a deja la décomposition
        //printf("deja presente direct pour %llu\n",n);
        if(isTheRightNumber(ndepart) == 1){
            return copyToDest(hashn, dest, nb_fact);
        }
    }
    
    
    for(;;)
	{
		if(is_prime(n) == 1)
		{
            pthread_mutex_lock(&lockTab);               //Lock file access
            tab[hash(n)][0] = n;
            pthread_mutex_unlock(&lockTab);            //Unlock file access
			dest[nb_fact] = n;
			nb_fact++;
			break;
		}
        else
		{
		uint64_t hasshedn = hash(n); 
            if(tab[hasshedn][0] != 0){//Si la décomp est deja presente, on la recupere
                if(isTheRightNumber(n) ==1){
                    //printf("deja presente pour %llu\n",n);
                    nb_fact = copyToDest(hasshedn, dest, nb_fact);
                    break;
                }else{
                    dest[nb_fact] = find_next_prime_factor(n);
                    n /= dest[nb_fact];
                    nb_fact++;
                }
            }else{
                    dest[nb_fact] = find_next_prime_factor(n);
                n /= dest[nb_fact];
                nb_fact++;
            }
		}
	}
	
    
    copyToTab(hashn, dest, nb_fact);
    
	return nb_fact;
}

void print_prime_factors(uint64_t n)
{
    /*printf("%llu : ",n);
     find_prime_factors(n);*/
    
    uint64_t factors[MAX_FACTORS];
    
    int j,k;
    
	k = get_prime_factors(n,factors);
	
	printf("%llu: ",n);
	for(j=0; j<k; j++)
	{
		printf("%llu ",factors[j]);
	}
	printf("\n");
    
    return;
}




//---------------------------------Methods------//
void open_file_and_find_prime_factors()
{
    FILE *f = fopen("numbers.txt", "r");
    
    uint64_t number;
    
    while(fscanf(f, "%llu",&number) != EOF) {
        print_prime_factors(number);
    }
	fclose(f);
}

void open_file_and_find_prime_factors_multithread()
{
    //open file
    FILE *f = fopen("numbers.txt", "r");
    
    uint64_t numberOne, numberTwo;
    
    while(fscanf(f, "%llu",&numberOne) != EOF && fscanf(f, "%llu",&numberTwo) != EOF ) { //read file
        pthread_t firstThread , secondThread;
        int crdu;
        
        
        //create first thread
        crdu = pthread_create(&firstThread,NULL,(void*)print_prime_factors,(void*)numberOne);
        if(crdu !=0)
            return;
        
        //create second thread
        crdu = pthread_create(&secondThread,NULL,(void*)print_prime_factors,(void*)numberTwo);
        if(crdu != 0)
            return;
        
        
        //Wait for threads to finish
        crdu = pthread_join(firstThread, NULL);
        crdu = pthread_join(secondThread, NULL);
        
        //print_prime_factors(number);
    }
    
	fclose(f);
}

void open_file_and_find_prime_factors_workerthread()
{
    pthread_t firstThread , secondThread, thirdThread;
    int crdu;
    
    
    
    FILE *f = fopen("numbers.txt", "r");      //open file
    
    pthread_mutex_init(&lock, NULL);     //create mutex in order to use fscanf safely
    pthread_mutex_init(&lockTab, NULL);
    
    
    //create first thread
    crdu = pthread_create(&firstThread,NULL,(void*)readNumber,(void*)f);
    if(crdu !=0)
        return;
    
    //create second thread
    crdu = pthread_create(&secondThread,NULL,(void*)readNumber,(void*)f);
    if(crdu != 0)
        return;
    
    
        //create first thread
    crdu = pthread_create(&thirdThread,NULL,(void*)readNumber,(void*)f);
    if(crdu !=0)
        return;
    



    //Wait for threads to finish
    crdu = pthread_join(firstThread, NULL);
    crdu = pthread_join(secondThread, NULL);
    crdu = pthread_join(thirdThread, NULL);

    
    
    pthread_mutex_destroy(&lock);  //destroy mutex
    pthread_mutex_destroy(&lockTab);
    fclose(f);
}

void readNumber(FILE *f)
{
    uint64_t number;
    int bytesRead = 0 ;
    
    
    for(;;){ //read file
        
        pthread_mutex_lock(&lock);              //Lock file access
        bytesRead = fscanf(f, "%llu",&number);  //read number from file
        //printf("TID : %d     %llu\n",pthread_self(),number);
        pthread_mutex_unlock(&lock);            //Unlock file access
        
        if(bytesRead != EOF)
            print_prime_factors(number);
        else
            break;
        
    }
    
    return;
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
    
    
    //open_file_and_find_prime_factors();
    //open_file_and_find_prime_factors_multithread();
    open_file_and_find_prime_factors_workerthread();
    
}
