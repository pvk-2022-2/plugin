#define MMIO_MASK  0xFF000000U

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef int int32_t;

volatile uint8_t* putch = (uint8_t*)(MMIO_MASK | 0x100);
volatile uint32_t* putint= (uint32_t*)(MMIO_MASK | 0x120);
volatile uint32_t* puthex= (uint32_t*)(MMIO_MASK | 0x130);

void puts(char* str) {
    #pragma nounroll
    while(*str)
        *putch = *str++;
}

// stolen from http://www.danielvik.com/2010/02/fast-memcpy-in-c.html
void* memcpy(void* dest, const void* src, uint32_t count) {
    char* dst8 = (char*)dest;
    char* src8 = (char*)src;

    while (count--)
        *dst8++ = *src8++;

    return dest;
}

//02-test for this gave an error on a teq instruction

// int is_prime(int num) {
	
//     if(num == 0 || num == 1) {
//         return 0;
//     }

// 	#pragma nounroll
// 	for (int i = 2; i < num; i++) {
//     	if(num % i == 0) {
//         	return 0;
//         }
// 	}
//     return 1;
// }

int is_prime(int num) {
	int isPrime = 1;

    if(num == 0 || num == 1) {
        isPrime = 0;
    }

	#pragma nounroll
	for (int i = 2; i < num; i++) {
    	if(num % i == 0) {
        	isPrime = 0;
        }
	}
    return isPrime;
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