#include "plugin_test.h"

void sort(uint32_t *list, uint32_t length) {
	int temp;
	int j;
	#pragma nounroll
	for (int i = 0; i < length; i++) {
		j = i;
		#pragma nounroll
		while (j > 0 && list[j - 1] > list[j]) {
			temp = list[j];
			list[j] = list[j - 1];
			list[j - 1] = temp;
			j--;
		}
	}
}

void __start() {
	uint32_t list[24] = {52, 12, 236, 634, 132346, 486, 263, 325, 574795, 3463, 35, 34, 568, 365234, 124, 124, 12534, 325, 124, 12446, 969, 54, 124, 32};
	const int length = sizeof(list) / 4;

	sort(list, length);

	#pragma nounroll
	for (int i = 0; i < length; i++)
	{
		*putint = list[i];
		*putch = ' ';
	}

	*putch = '\n';

	#pragma nounroll
	for (int i = 0; i < length - 1; i++)
		if (list[i] > list[i + 1]) {
			puts("TEST FAILED!\n");
			return;
		}

	puts("TEST PASSED!\n");
}