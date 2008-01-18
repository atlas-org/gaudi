/*
* $Id: memget.c,v 1.6 2004/09/14 16:36:50 mato Exp $
*/
/*CMZ :  2.00/01 09/01/2000  21.14.37  by  Pavel Nevski*/
/*-- Author :    Mark Hsu 2/1/91 HPCSD, Kingston, NY.*/
/* memget.c: allow dynamic memory allocation from FORTRAN
 * Mark Hsu 2/1/91 HPCSD, Kingston, NY.
 * usage from FORTRAN:    I = memget (n)
 *                        J = memgetf(n)
 * where n is the number of bytes (words) requested
 *       I (returned value) - the base address (bytes)
 * or    J (returned value) - the base address in word
 * Comments:
 * malloc() requires unsigned arg. FORTRAN passes signed integers
 * malloc() returns a pointer;     memget(f) returns an integer.
 */
#include <stdlib.h>

#ifdef WIN32
unsigned long __stdcall MEMGET (unsigned int* n)
#else
unsigned long memget_  (unsigned int *n)
#endif
{  return ( (unsigned long) malloc(*n) );  }

#ifdef WIN32
unsigned long __stdcall MEMGETF (unsigned int* n)
#else
unsigned long memgetf_  (unsigned int *n)
#endif
{  return ( (unsigned long) malloc(*n<<2)>>2 ); }

