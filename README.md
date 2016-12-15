#IAR-STM32F4xx-Mixed_C-AsmT
This code is compatible with the IAR Embedded Workbench targeting STM32F4xx project. It is mixed C and assembly.  I would redo it, using the scratch registers more when I could as opposed to pushing the non-scratch, using those, then popping.  But it all works and I have moved on, but you get all the basic concepts demonstrated: mov, ldr, str, load and store multiple, IT blocks, branch to adress store in register (BLX), also the directives you would need for GCC (conditionally compiled).

This project features the following functions is assembly:

-----------------------------------------------------------------
gen_sort: a generic sort routine that will sort any type of data.  The test code included sorts integer array, integer grid, string array, and array of structs with two sorting criteria, name and grade point average.  It uses a non-conventional means in that it always increments the array offset by word size (same as pointer size)  If you want to see a more conventional way to write generic C code, see my other repository C-Algo.  To see generic programming in C++, see CPP-Algo.

Here is the C declaration for gen_sort:

void gen_sort(int* a, const size_t count, int (*pred)(const int, const int));

Parameters: pointer to contiguous memory, size in words, a predicate functor
that implements the sorting criteria.

int_less and int_notless are included in this file that you can use for the predicate:

gen_sort(array, array_size, int_less);  // this will sort an array of ints in ascending.

-------------------------------------------------------------------

icopy: copies the contents of one buffer to another buffer.
C declaration:  
void icopy(int* restrict array1, int* restrict array2, const size_t len); 

Parameters: pointer to a destination buffer, pointer to a source integer array, 
size of the source array. 

Requirements: 
1.) the destination buffer must be large enough as to not overflow
2.) if the destination and source overlap, the results are undefined.

----------------------------------------------------------------------
is_equal: compares the element values of two ranges of contigous memory.
C declaration:  
bool isequal(int* array, const size_t len) 

Parameters: pointer to integer array, number of elements

Returns true or false

--------------------------------------------------------------------------
transform: modifies a range of elements with a user provided functor. The transform.s
file includes rand_wrapper functor.

void transform(int* a, const size_t count, int (*pred)(int));

Parameters: pointer to integer array, number of elements, function pointer that
modifies the elements.  Example, the following populates the array with random
values.

transform(a, _countof(a), rand_wrapper);  

--------------------------------------------------------------------------------
main.c is the test driver which tests all of the functionality and is instructive on how to use the assets.
After the tests have run, it loops blinking the led.

