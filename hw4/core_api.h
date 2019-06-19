/* 046267 Computer Architecture - Spring 2019 - HW #4 */
/* Main memory simulator implementation               */

#ifndef CORE_API_H_
#define CORE_API_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <stdbool.h>

#define REGS 8

typedef enum {
    CMD_NOP = 0,
    CMD_ADD,    // dst <- src1 + src2
    CMD_SUB,    // dst <- src1 - src2
    CMD_ADDI,    // dst <- src1 + imm
    CMD_SUBI,    // dst <- src1 - imm
    CMD_LOAD,   // dst <- Mem[src1 + src2]  (src2 may be an immediate)
    CMD_STORE,  // Mem[dst + src2] <- src1  (src2 may be an immediate)
	CMD_HALT,
} cmd_opcode;

typedef struct _latency{
	int Load_latecny;
	int Store_latency;
}Latency;

 typedef struct _inst {
	cmd_opcode opcode;
	int dst_index;
	int src1_index;
	int src2_index_imm;
	bool isSrc2Imm; //if the second argument is immediate
}Instuction;

typedef struct _regs {
	int reg[8];
}tcontext;

typedef enum {
	Success,
	Failure,
} Status;



/* function that simulates Blocked multithreading functionality
 * param[in] the load store latecny the first elemnt is the load latecny and the second is store latency
 * this function must return Success
 */
Status Core_blocked_Multithreading();

/* function that simulates finegrained  multithreading functionality
 * param[in] the load store latecny the first elemnt is the load latecny and the second is store latency
 * this function must return Success
 */
Status Core_fineGrained_Multithreading();

/* functions that returns the context of each thread for each of the  multithreading type
 * param[in] is of type tcontext to have the context of each thread after the call
 */
Status Core_blocked_context(tcontext blocked_context[],int threadid);
Status Core_finegrained_context(tcontext finegrained_context[], int threadid);

/* functins that returns the CPI for each of the multithreading type
 */
double Core_finegrained_CPI();
double Core_blocked_CPI();

#ifdef __cplusplus
}
#endif

#endif /* CORE_API_H_ */
