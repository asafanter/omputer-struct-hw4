/* 046267 Computer Architecture - Spring 2019 - HW #4 */
/* API for the multithreading processor simulator */

#ifndef _SIM_API_H_
#define _SIM_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>



/*************************************************************************/
/* The memory simulator API - implemented in sim_mem.c                   */
/*************************************************************************/
//freeing all the allocations
int SIM_MemFree();

/*! SIM_MemReset: Reset the memory simulator and load memory image
  \param[in] memImgFname Memory image filename
  The memory image filename is composed from segments of 2 types, defined by an "@" location/type line:
  1. "I@<address>" : The following lines are instructions at given memory offset.
     Each subsequent line up to the next "@" line is an instruction of format: <command> <dst>,<src1>,<src2>
     Commands is one of: NOP, ADD, SUB, LOAD, STORE
     operands are $<num> for any general purpose register, or just a number for immediate (for src2 only)
  2. "D@<address>" : The following lines are data values at given memory offset.
     Each subsequent line up the the next "@"is data value of a 32 bit (hex.) data word, e.g., 0x12A556FF
  \returns 0 - for success in reseting and loading image file. <0 in case of error.

  * Any memory address that is not defined in the given image file is initialized to zero.
 */
int SIM_MemReset(const char *memImgFname);

/*! SIM_ReadDataMem: Read data from main memory simulator
  \param[in] addr The memory location to read.
                  Note that while we read 32 bit data words, addressing is per byte, i.e., the address must be aligned to 4.
  \param[out] dst The destination location to read into
  \returns 0 - for success in reading. (-1) for memory wait-state
  * In case of a wait-state error code, one should invoke again on the next clock cycle.
*/
int SIM_MemDataRead(uint32_t addr, int32_t *dst);

/*! SIM_MemDataWrite: Write a value to given memory address
  \param[in] addr The main memory address to write. Must be 4-byte-aligned
  \param[in] val  The value to write
*/
void SIM_MemDataWrite(uint32_t addr, int32_t val);

// TODO - add description
/*! SIM_ReadInstMem: Read instruction from main memory simulator
  \param[in] addr The memory location to read.
                  Note that while we read 32 bit data words, addressing is per byte, i.e., the address must be aligned to 4.
  \param[out] dst The destination location to read into
*/
void SIM_MemInstRead(uint32_t addr, Instuction *dst, int id);


/*/returns the latency for load and store instructions
	\param[out] array with two elements the first has the load latency, and the second has the store latecny
	\return 0 in case if success
	*/
int Mem_latency(int *);

// returns how many threads are running in the program
int Get_thread_number();

// retrun the cycles for the switch
int Get_switch_cycles();

#ifdef __cplusplus
}
#endif

#endif /*_SIM_API_H_*/
