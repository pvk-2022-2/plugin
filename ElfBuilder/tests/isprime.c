#include "plugin_test.h"

int is_prime(int num) {
    if(num == 0 || num == 1) {
        return 0;
    }

	#pragma nounroll
	for (int i = 2; i < num; i++) {
    	if(num % i == 0) {
        	return 0;
        }
	}
    return 1;
}

void __start() {
	uint32_t primes[10] = {2,3,5,7,11,13,17,29,23,29};
	uint32_t not_primes[10] = {0,1,4,6,8,22,33,144,25,26};
	uint32_t length = 10;

	int count1 = 0;
	int count2 = 0;

	#pragma nounroll
	for(int i = 0; i < length; i++) {
		count1 += is_prime(primes[i]);
	}

	#pragma nounroll
	for(int i = 0; i < length; i++) {
		count2 += is_prime(not_primes[i]);
	}

	if(count1 == 10 && count2 == 0) {
		puts("TEST PASSED!\n");
	} else {
		puts("TEST FAILED!\n");
	}
}