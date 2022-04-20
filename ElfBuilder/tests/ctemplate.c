#define MMIO_MASK  0xFF000000U

typedef unsigned int uint32_t;
typedef int int32_t;

volatile uint32_t* putch = (uint32_t*)(MMIO_MASK | 0x100);
volatile uint32_t* putint= (uint32_t*)(MMIO_MASK | 0x120);
volatile uint32_t* puthex= (uint32_t*)(MMIO_MASK | 0x130);

void puts(char* str) {
    #pragma nounroll
    while(*str)
        *putch = *str++;
}


void __start() {
    

    puts("TEST PASSED!\n");
}