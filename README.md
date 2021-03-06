# LKMALLOC

**Lkmalloc** is a debugging API that wraps around `malloc(3)` and `free(3)`. The goal of this library is to track all calls to `malloc(3)` and `free(3)` and describe any potential memory leaks, bugs, or invalid free's.

## Makefile

Use the Makefile to perform different actions.

```bash
make
make clean
make lib
make test
```
To clean up all object files, dependencies, and binaries, run...
```bash
make clean
```
To compile only the static library, run...
```bash
make lib
```
This will create an archive (.a) file containing all of the library's code.  

To compile both the static library and driver programs, run...
```bash
make all
```

To run the test scripts, run...
```bash
make test
```
To install this library, please include paths to the `lkmalloc.h` header file and the `liblkmalloc.a` archive. Be sure to compile with the with the LDFLAGS `-L<path to archive> -llkmalloc`.

## Structure
The basic file tree looks like the one below. All files associated with  **lkmalloc** are in the `lib` folder. All files in the `driver` folder are meant to showcase usage of the library. 

	├── driver
	│   ├── bin
	│   │   ├── *.out
	│   ├── build
	│   │   ├── *.d
	│   │   ├── *.o
	│   ├── include
	│   │   └── driver.h
	│   └── src
	│       ├── driver1.c
	│       ├── driver2.c
	│	├── driver3.c
	│       └── driver4.c
	├── lib
	│   ├── build
	│   │   ├── *.d
	│   │   ├── *.o
	│   ├── include
	│   │   ├── lkmalloc.h
	│   │   ├── rbtree.h
	│   │   └── record.h
	│   ├── liblkmalloc.a
	│   └── src
	│       ├── lkmalloc.c
	│       ├── rbtree.c
	│       └── record.c
	├── Makefile
	├── README
	└── tests
	    ├── test1.sh
	    ├── test2.sh
	    └── test3.sh


## Usage
### NAME
lkmalloc, lkfree, lkreport - track and report dynamic memory allocations.
### SYNOPSIS

```C
#include "lkmalloc.h"

int lkmalloc(unsigned int size, void **ptr, unsigned int flags);
int lkfree(void **ptr, unsigned int flags);
int lkreport(int fd, unsigned int flags);
```
### DESCRIPTION
The __lkmalloc()__ function allocates *size* bytes and stores the address to the allocated memory into **ptr*.  It does so by making a call to __malloc()__. The __lkmalloc()__ function will also log a record of the call and internally store relevant information; see __lkrecord__ for details. Additional behavior and protections are invoked based on the *flags* passed in; introducing memory initialization and redzones.

The full list of __lkmalloc__ flags is as follows:

__LKM_REG__
Do not add invoke additional behavior or protections.

__LKM_INIT__
Initialize all of the allocated memory with the NULL byte.

__LKM_OVER__
Add an additional 8 bytes of memory at the end of the allocation and writes the special pattern *0x5a*.

__LKM_UNDER__
Add an additional 8 bytes of memory at the beginning of the allocation and writes the special pattern *0x6b*.

>__NOTE:__ __LKM_REG__ may not be combined with any other flag. If combined, __LKM_REG__ is ignored and __lkmalloc()__ will invoke the other flags combined.

The __lkfree()__ function frees the memory space pointed to by **ptr*, which must have been written by a previous call to __lkmalloc()__.  It does so by making a call to __free()__, only if the **ptr* passed in is a valid allocated address in memory. The __lkfree()__ function will also log a record of the call and internally store relevant information; see __lkrecord__ for details. Additional behavior and protections are invoked based on the *flags* passed in. This will attempt to handle bad frees's, orphaned frees, double frees, and misaligned addresses of an allocated space.

The full list of __lkfree__ flags is as follows:

__LKF_REG__
Free a memory allocation if and only if **ptr* matches the address returned by __lkmalloc()__. 

__LKF_APPROX__
Free a memory allocation if **ptr* is an approximate address returned by __lkmalloc()__. **ptr* can point to any part of the allocated block and will be properly freed.

__LKF_WARN__
Print a warning to `STDERR` if a memory allocation was approxmiately freed as per __LKF_APPROX__ . 

__LKF_UNKNOWN__
Print a warning to `STDERR` if **ptr*  is an invalid address not allocated in memory. 

__LKF_ERROR__
This will `exit(3)` the program if either conditions in __LKF_WARN__ or __LKF_UNKNOWN__ are met.

>__NOTE:__ __LKF_REG__  and __LKF_APPROX__ may not be combined with each other. If combined, __lkfree()__ will favor __LKF_APPROX__. It is recommended to combine __LKF_WARN__  with __LKF_APPROX__ in order to print the warning. Otherwise, __LKF_WARN__  will be ignored. 

The __lkreport()__ function will dump a report of all of the records that were logged in __lkmalloc()__ and __lkfree()__.  The report will be written to the file referred to by the file descriptor *fd* and perform clean up operations on the internal data structures as well as free memory allocations that were leaked by the caller. The kind of reports that are dumped by __lkreport__() are defined by the *flags* passed in. 

The full list of **lkreport** flags is as follows:

**LKR_NONE**  
Do not print a report of the records tracked.

**LKR_SERIOUS**  
Print records that are considered memory leaks(e.g __lkmalloc()__ calls that were not freed by the caller).

**LKR_MATCH**  
Print records whose memory address have a corresponding call to __lkmalloc()__ and __lkfree()__ and the address freed matches the address returned by __lkmalloc()__.

**LKR_BAD_FREE**  
Print records that attempt to free memory addresses that did not point to their original allocation from __lkmalloc()__(e.g. memory addresses pointing in other locations of an allocated block).
 
**LKR_ORPHAN_FREE**  
Print records that attempt to free memory addresses that were never, in the history of the program, allocated by __lkmalloc()__.

**LKR_DOUBLE_FREE**  
Print records that attempt to free memory addresses were already freed by __lkfree()__.

**LKR_APPROX**  
Print records whose memory address have a corresponding call to __lkmalloc()__ and __lkfree()__ and the address freed was pointed to some allocated address in the allocated block (as per __LKF_APPROX__).

>__NOTE:__ __LKR_NONE__ may not be combined with any other flag. If combined, the __LKR_NONE__ flag is ignored and __lkreport()__ will invoke the other flags combined.

#### LKRECORD
An lkrecord will record generic information such as..

 - record type(*lkmalloc* or *lkfree*)
 - caller's file name
 - caller's function_name
 - caller's line number
 - UNIX timestamp
 - pointer passed in
 - return value
 
 If the record is of type *lkmalloc*, additional information is recorded; such as...
 
 - actual allocation address
 - address returned to the caller
 - real size of the allocated space
 - requested size of memory from the caller

 If the record is of type *lkfree*, additional information is recorded; such as...
 
 - flags passed into __lkfree()__ by the caller
 - internal flags to define a valid/invalid free
 - a pointer to a lkmalloc record corresponding to its free
### RETURN VALUE
__lkmalloc()__ and __lkfree()__ will return 0 on success. On error, a negative error status code is returned describing the type of error.

__lkreport()__ will return the number of records printed on success. On error, a negative error status code is returned describing the type of error.

### ERRORS
__lkmalloc()__, and __lkfree()__ can fail with the following error:

__EINVAL__ Invalid arguments

__lkmalloc()__ can fail with the following error:

__ENOMEM__ Out of memory. __lkmalloc()__ failed to allocate the requested memory.

__lkreport()__ can fail with the following errors:

__EBADF__ *fd* is not a valid file descriptor.

__EACCES__ *fd* is not open for writing.

If any of the internal recording functions face a fatal error, the API will print an error to `STDERR` and make a call to __exit(3)__ with the exit code a negative error status code.
### NOTES

The API will only record/log calls to __lkmalloc()__ and __lkfree()__ and nothing more. The API will not attempt to NULL pointers or correct address values passed in. Any record reported as an error must be addressed by the caller.

__lkmalloc()__ and __lkfree()__ are wrappers around `malloc(3)` and `free(3)` and therefore should __NOT__ be mixed together in a program. Pointers returned by `malloc(3)`  and passed into __lkfree()__, or vice versa, will produce unexpected behavior.

__lkreport()__ will dump all records at the current snapshot of the program. Calling __lkreport()__ at different parts of the program with different flags can lead to missing out on certain information(due to the function clearing out its internal data structures for record keeping). Rather than calling __lkreport()__ multiple times, it is recommeded to use `atexit(3)` to register the function to be called on exit.  

A wrapper function can defined in order to achieve this (see `driver.h`). It is best to set up theses wrapper  functions within the user's program. Then user can then call these wrappers at the beginning of the program to register __lkreport()__ as an exit function.  This way, the API will always dump an accurate report and properly clean up memory despite the exit condition.

>Note: If a signal (like SIGSEGV) is sent to the program, functions registered with `atexit(3)` will not be called.

Here is an example of the wrapper functions:
```C
/* global variables for setting up lkreport */
int lkreport_fd;
unsigned int lkreport_flags;

/* void function use to call lkreport at exit */
void lkreport_atexit()
{
	lkreport(lkreport_fd, lkreport_flags);
}

/* setter to init the variables for lkreport and call it with atexit(3) */
void set_lkreport(int fd, unsigned int flags)
{
	lkreport_fd = fd;
	lkreport_flags = flags;
	atexit(lkreport_atexit);
}
```
Finally, the wrapper can be called in the beginning of the user program...
```C
int main(){
	int my_fd = open(<my file>);
	set_lkreport(my_fd, <lkreport flags>);
	...
```

The records printed out by __lkreport()__ will be in a .csv format(comma seperated value) and can be opened with other csv programs. Records are ordered based on how they are arranged in the internal data structures. If you wish to sort them, please open the .csv report in a editor/viewer program (Microsoft Excel, Google Sheets, etc).
## Basic information & Implementation
This library wraps around `malloc(3)` and `free(3)` and make subsequent calls to those functions on behalf of the caller. The library also allocates memory for its internal data structure. The  records are defined in a 	`struct` that is packed in other `structs`(see record.h).

```C
/* lkrecord */

struct generic
{
	unsigned  int record_type : 1;
	const  char *file_name;
	const  char *function_name;
	int line_num;
	unsigned  long time;
	void *ptr_passed;
	int retval;
};

struct  malloc_extension
{
	void *real_ptr;
	void *addr_returned;
	unsigned  int real_size;
	unsigned  int requested_size;
};

struct  free_extension
{
	unsigned  int flags_passed;
	unsigned  int internal_flags;
	struct  lkrecord  *malloc_pair;
};

struct  lkrecord_data
	{
	struct  generic  generic_info;
	union {
		struct  malloc_extension  malloc_info;
		struct  free_extension  free_info;
	};
};

struct  lkrecord
{
	struct  lkrecord_data  data;
	struct  lkrecord  *next;
};
```
The type of data structure used is a combination of a red-black tree with nodes pointing to a linked list. This is otherwise described as a TreeMap(see rbtree.h). 

`N` is the tree node that points to a linked list of records `R`.

		 N
		/ \
	       /   \
	      N	    N                   +        N -> R -> R -> R -> R
	     / \
	    /   \
	   N	 N

The nodes in the tree have keys equal to varies memory addresses. Each node has a list of records for that specific memory address over the programs lifetime. TreeMaps have very efficient lookup times, which is used often in the API to track any memory bugs in the caller's program.  A red black tree was chosen specifiically because of its rebalancing guidelines.  The tree will have nodes with keys of very high values, so it susepitble to be imbalanced. In order to keep its efficiency during access times, the tree must remain balanced.

Internally, the API stores records in two seperate trees. One tree is for all malloc records that are  currently allocated by the caller; which is referred internally as the `m_tree`. The other tree is for all free records made by the caller; which is referred internally as the `f_tree`. 

When a call to __lkmalloc()__ happens, a new malloc record  is pushed onto the `m_tree`, inserting a new tree node as well.  

When a call to __lkfree()__ happens, the API will look in the `m_tree` for the allocated address to be freed. If a node was found, the tree node is removed from the `m_tree` and the malloc record is popped from the node. The new free record will save the pointer of the popped malloc record as its "malloc pair".  If a node was not found, the "malloc pair" is NULL.

The new free record is then push onto the `f_tree`and appended to the node whose key is equal to the allocated address passed in. If there is no node whose key matches the address passed in, a new tree node is inserted. 

In the worst case, the lookup time will take  `O(log(n) +  r` where `n` is the number of nodes in the tree and `r` is the highest number of records pointed at a node.

## Resources

Functions and code in this program were inspired by these references.

*https://en.wikipedia.org/wiki/Red%E2%80%93black_tree*
*https://www.geeksforgeeks.org/red-black-tree-set-3-delete-2/?ref=lbp*
*https://www.geeksforgeeks.org/c-program-red-black-tree-insertion/*
*https://github.com/bvalarezo/red-black-tree/blob/master/rbtree.h* (This is my personal Github)
## Contributing
Maintained by Bryan Valarezo

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

[MIT](https://choosealicense.com/licenses/mit/)
