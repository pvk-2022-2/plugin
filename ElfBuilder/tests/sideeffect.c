#define MMIO_MASK 0xFF000000U

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef int int32_t;

volatile uint8_t *putch = (uint8_t *)(MMIO_MASK | 0x100);
volatile uint32_t *putint = (uint32_t *)(MMIO_MASK | 0x120);
volatile uint32_t *puthex = (uint32_t *)(MMIO_MASK | 0x130);

void puts(char *str) {
	#pragma nounroll
	while (*str)
		*putch = *str++;
}

uint32_t global_var = 0;

void side_effect() {
	global_var++;
}

void __start()
{
	side_effect();
	side_effect();
	side_effect();
	side_effect();

	*putint = global_var;

	*putch = '\n';


	if (global_var == 4) {
		puts("TEST PASSED!\n");
	} else {
		puts("TEST FAILED!");
	}
}