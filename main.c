/*****************************************************************************
FILE NAME   : main.c
DESCRIPTION : Lab 04 - Assembly Language Programming.
COURSE      : EMBSYS100, 2016
STUDENT     : <Your Name>
******************************************************************************/

#include "NucleoBoard.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#ifndef SIXTEEN
        #define SIXTEEN 16
#endif
#ifndef TEN
        #define TEN     10
#endif
typedef int IntType;
typedef IntType value_type;
typedef value_type *pointer;
typedef pointer iterator;
typedef const iterator const_iterator;
typedef void* genptr;
typedef int   pointer_or_int;
typedef int   int_holding_const_char_pointer;

#ifndef grid_count
        #define grid_count(x) sizeof(x)/sizeof(x[0][0])
#endif

/* assembly declarations */
void gen_sort(genptr array, const size_t count, 
              int (*pred)(const pointer_or_int, const pointer_or_int));

void transform(pointer array, const size_t count, 
               value_type(*pred)(value_type));

bool is_equal(pointer array1, pointer array2, const size_t count);
void icopy(pointer restrict dest, pointer restrict src, const size_t count); 
int rand_wrapper(const value_type); 

int int_less(value_type v1, value_type v2);
int int_notless(value_type v1, value_type v2);
int gen_str_less(int_holding_const_char_pointer s1, 
                 int_holding_const_char_pointer s2);

/* the test driver */
void test_c();
void print_array(pointer array, const size_t count);
///////////////////////////////////////////////////////////////////////////////
//                                  M a i n
///////////////////////////////////////////////////////////////////////////////

void main()
{
    HardwareInit();
    StartSystemTimer();
    
    // Initialize the serial port with the specified baud rate.
    USART2_Init(9600);

    // Print the caption.
    puts("\nLab Week 4: Assembly Language Programming.");
    printf("Built %s %s.\n\n", __DATE__, __TIME__);

    test_c();

    while (1)
    {
        GPIO_WriteBit(GPIO_PORT_LD2, GPIO_PIN_LD2, Bit_SET);
        BusyWaitMilliSeconds(20);

        GPIO_WriteBit(GPIO_PORT_LD2, GPIO_PIN_LD2, Bit_RESET);
        BusyWaitMilliSeconds(5000 - 20);
    }
} // main.

void test_c()
{
        puts("\n-----------TEST 1: testing the tests: icopy and is_equal---\n");
        int expected[]= {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1, 0};
        int buf[SIXTEEN];
        icopy(buf, expected, _countof(buf));
        assert(is_equal(expected, buf, _countof(expected)));
        print_array(buf, _countof(buf));
        printf("--PASSED assert %d, %s, %s---\n", __LINE__, __FILE__, __func__);
  
        puts("\n TEST 2:  Sort the Lab assignment array -------------------\n");
        int TheLabRandom[] = {
                -41, 30, -120, 2, 108, 0, 126, 1, 3, 109, -4, 25, 6, -100, 65};
   
        gen_sort(TheLabRandom, _countof(TheLabRandom), int_less);
        puts("ascending\n");
        print_array(TheLabRandom, _countof(TheLabRandom));
        gen_sort(TheLabRandom, _countof(TheLabRandom), int_notless);
        puts("ascending\n");
        print_array(TheLabRandom, _countof(TheLabRandom));
        
        puts("\n-----------TEST 3: sort random numbers ascending-----------\n");  
        int test[SIXTEEN];
        int compare[SIXTEEN];
        srand(97);
        
        transform(test, _countof(test), rand_wrapper);
        icopy(compare, test, _countof(compare));
        
        gen_sort(test, _countof(test), int_less);
        gen_sort(compare, _countof(compare), int_less);
        
        assert(is_equal(test, compare, _countof(test)));
        print_array(test, _countof(test));
        
        printf("--PASSED assert %d, %s, %s---\n", __LINE__, __FILE__, __func__);
     
        puts("\n-----------TEST 4: sort random numbers descending----------\n"); 
        srand(43);
        transform(test, _countof(test), rand_wrapper);
        icopy(compare, test, _countof(compare));
        
        gen_sort(test, _countof(test), int_notless);
        gen_sort(compare, _countof(compare), int_notless);
        
        assert(is_equal(test, compare, _countof(test)));
        print_array(test, _countof(test));
        printf("--PASSED assert %d, %s, %s---\n", __LINE__, __FILE__, __func__);
       
        puts("\n-----------TEST 5: sort grid of random numbers ascending---\n"); 
        srand(1);
        int grid_test[TEN][TEN];       
        int grid_compare[TEN][TEN];
        int *ptest = (int*)&grid_test;
        int *pcmp = (int*)&grid_compare;
        
        transform(ptest, grid_count(grid_test), rand_wrapper);
        icopy(pcmp, ptest, grid_count(grid_compare));
        puts("before\n");
        
        for(size_t row=0; row < TEN; row++) {
                for(size_t col=0; col < TEN; col++)
                        printf("%d ", grid_test[row][col]);
                               
                puts("\n");
        }
        
        gen_sort(grid_test, grid_count(grid_test), int_less);
        gen_sort(grid_compare, grid_count(grid_compare), int_less);       
        assert(is_equal(ptest, pcmp, grid_count(grid_test)));
        puts("after\n");
        for(size_t row=0; row < TEN; row++) {
                for(size_t col=0; col < TEN; col++)
                        printf("%d ", grid_test[row][col]);
                               
                puts("\n");
        }
        printf("--PASSED assert %d, %s, %s---\n", __LINE__, __FILE__, __func__);
        
        puts("\n-----------TEST 6: sort string array lexical order---------\n"); 
        const char* strs[] = {
                "zero", "one", "two", "three", "four", "five", "six", "seven",
                "eight", "nine", "ten", "eleven", "twelve", "thirteen",
                "fourteen", "fifteen"
        };
                
        gen_sort(strs, _countof(strs), gen_str_less);
        
        for(size_t i=0; i < _countof(strs); i++)
                printf("%s\n", strs[i]);
        
     
        printf("--PASSED assert %d, %s, %s---\n", __LINE__, __FILE__, __func__);       
}
/* predicate for sorting strings */
int gen_str_less(int_holding_const_char_pointer s1, 
                 int_holding_const_char_pointer s2)
{
 /* Warning: diabolical code!!  Need to make sure that word size is same 
 as pointer size due toe vil storing of const char* cast into int for the 
 generic sorting predicate so that we can use the same signature for both
 word size and pointer size predicate arguments */
        assert(sizeof(int) == sizeof(const char*));
        assert(sizeof(int) == sizeof(int*));
        assert(s1 && s2);
        const char* left = (const char*)s1;
        const char* right = (const char*)s2;
        return strcmp(left, right) < 0 ? 1 : 0;
}
void print_array(pointer array, const size_t count)
{
        assert(array);
        for(pointer p=array; p < (array + count); p++)
                printf("%d ", *p);
        
        puts("\n");
}

