#include "plugin_test.h"

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