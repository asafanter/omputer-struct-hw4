/* 046267 Computer Architecture - Spring 2019 - HW #4 */

#include<stdio.h>
#include "core_api.h"
#include "sim_api.h"

int main(int argc, char const *argv[]){
	 char const *memFname = argv[1];
	    if (SIM_MemReset(memFname) != 0) {
	        fprintf(stderr, "Failed initializing memory simulator!\n");
	        exit(2);
	    }
	int threads = Get_thread_number();
	tcontext blocked[threads];
	tcontext finegrained[threads];
	for(int k=0; k < Get_thread_number(); k++){
	    for (int i = 0; i < REGS; ++i){
	        finegrained[k].reg[i]=0;
	    	blocked[k].reg[i]=0;
	    }
	}
	Core_blocked_Multithreading();
	printf("\n----Blocked Simulation -------\n");
	for(int k=0; k < Get_thread_number(); k++){
		Core_blocked_context(blocked,k);
	    printf("\nRegister file thread id %d:\n",k);
	    for (int i = 0; i < REGS; ++i)
	        printf("\tR%d = 0x%X", i, blocked[k].reg[i]);
	}
    printf("\nBlocked Multithreading CPI for this program %lf\n",	Core_blocked_CPI());

	Core_fineGrained_Multithreading();
	printf("\n-----Finegrained Simulation -----\n");
	for(int k=0; k < Get_thread_number(); k++){
		Core_finegrained_context(finegrained,k);
	    printf("\nRegister file thread id %d:\n",k);
	    for (int i = 0; i < REGS; ++i)
	        printf("\tR%d = 0x%X", i, finegrained[k].reg[i]);
	}
	printf("\nFinegrained Multithreading CPI for this program %lf\n", Core_finegrained_CPI());
	SIM_MemFree();
	return 0;
}


