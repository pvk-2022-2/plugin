#include "plugin_test.h"

void __start() {
    
    uint32_t list[24] = {52,12,236,634,132346,486,263,325,574795,3463,35,34,568,365234,124,124,12534,325,124,12446,969,54,124,32};
    const int length = sizeof(list) / 4;
    
    uint32_t swap;
    
    // Nounroll prevents O2 from predicting test pass 
    #pragma nounroll
    for(int i = 0; i < length; i++)
        for(int j = 0; j < length - i - 1; j++)
            if(list[j] > list[j + 1]){
                swap = list[j];
                list[j] = list[j + 1];
                list[j + 1] = swap;
            }
    
    #pragma nounroll
    for(int i = 0; i < length; i++) {
        *putint = list[i];
        *putch = ' ';
    }

    *putch = '\n';

    #pragma nounroll
    for(int i = 0; i < length - 1; i++)
        if(list[i] > list[i + 1])
        {
            puts("TEST FAILED!\n");
            return;
        }

    puts("TEST PASSED!\n");
}