
/*BEGIN_LEGAL 
Intel Open Source License 

Copyright (c) 2002-2016 Intel Corporation. All rights reserved.
 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */
/*
 *  This file contains an ISA-portable PIN tool for tracing system calls
 */


#define NDEBUG

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include "pin.H"

#include <sys/time.h>
#include <errno.h>
#include <signal.h>

typedef struct sysent {
		unsigned nargs;
		int	sys_num;
		const char *sys_name;
} struct_sysent;

const struct_sysent sysent[] = {
#include "syscall_list.h"
};

FILE * trace;
unsigned long sys_arg[6];
unsigned long syscall_num;
bool isMainProgram = false;
char newbuffer[250];
//char newbuffer2[250];
unsigned int seek=0;
int pid;
KNOB<BOOL>   KnobReplay(KNOB_MODE_WRITEONCE,  "pintool",
    "replay", "0", "count instructions, basic blocks and threads in the application");

VOID MainBegin()
{
		isMainProgram = true;
}

VOID MainReturn()
{
		isMainProgram = false;
}

//Replacement Function for Rand()
INT32 MyRandFunc()
{			
		
						while(true)
				   {
					     fgets(newbuffer,250,trace);
						    if(strstr(newbuffer,"rand:"))
						     break;
						   }

 printf("[PINTOOL] rand(): returns %s\n",newbuffer+5);
			return atoi(newbuffer+5);
				
}

VOID RandReturn(ADDRINT ret)
{
		if(KnobReplay)
		{
	}
		else
		{
		printf("[PINTOOL] rand(): returns %ld\n", ret);
		fprintf(trace, "rand: %ld\n",ret);
}
}

VOID CheckforSignal(){

		if(strstr(newbuffer,"signal:")){
					printf("signal=%s",newbuffer+7);	
				if(atoi(newbuffer+7)==2)
						exit(0);
		}
		
}

static VOID SysBefore(ADDRINT ip, ADDRINT num, ADDRINT arg0, ADDRINT arg1, ADDRINT arg2, ADDRINT arg3, ADDRINT arg4, ADDRINT arg5, CONTEXT *ctxt, ADDRINT nextAddr)
{
		if(!isMainProgram) return;

		syscall_num = num;
		sys_arg[0] = (unsigned long)arg0;
		sys_arg[1] = (unsigned long)arg1;
		sys_arg[2] = (unsigned long)arg2;
		sys_arg[3] = (unsigned long)arg3;
		sys_arg[4] = (unsigned long)arg4;
		sys_arg[5] = (unsigned long)arg5;
		ADDRINT retval=0;
//		ADDRINT readbuffer=0;

//void *destbuffer= "init";
		size_t size=1024;
		int count=0;
	//	char *sysname;
		if(KnobReplay) {
//				fseek(trace,7,SEEK_SET);
						fgets(newbuffer,250,trace);
				CheckforSignal();
				if(strstr(newbuffer,"sys:")){
			
		//	int sign =CheckforSignal();
			
			char	*split;
						if(strcmp(sysent[syscall_num].sys_name,"read")==0){

						split=strtok(newbuffer,":");
								while(split!=NULL)
								{
										//printf("split%s\n",split);
										count++;
						
						/*				if(count==4){
													//	readbuffer=atol(split);
								char *ptr;
								//ptr=split;
								printf("buffaddr=%d\n",*(char*)split);
										}*/
										if(count==5){
										size=(size_t)atoi(split);
										printf("size=%lu",size);
										}
										if(count==9){
										retval=(ADDRINT)atoi(split);
										printf("retval%ld\n",retval);
										}
										split=strtok(NULL,":");
								}
					
								///	memcpy((void *)sys_arg[1],(void *)readbuffer,size);
								//	PIN_SetContextReg(ctxt,REG_RAX,retval);
								//printf("sys_arg:%s\n",(char*)sys_arg[1]);
					}
								//PIN_SetContextReg(ctxt,REG_INST_PTR,nextAddr);
								//PIN_ExecuteAt(ctxt);							
	//CheckforSignal();
}

		//							printf("[PINTOOL] Replay: syscall %ld(%s)\n", syscall_num, sysent[syscall_num].sys_name);
			//					}
				//		}
		}
		
}

VOID SysAfter(ADDRINT ret, ADDRINT err)
{
		if(!isMainProgram) return;
		
		if(!KnobReplay) {
				//printf("[PINTOOL] Recording: syscall %ld(%s), ret %ld, err %ld\n", syscall_num, sysent[syscall_num].sys_name, ret, err);
				//if(syscall_num==0)
				fprintf(trace,"sys:%s:%ld:0x%lx:%ld:%ld:%ld:%ld:%ld\n",sysent[syscall_num].sys_name,sys_arg[0],sys_arg[1],sys_arg[2],sys_arg[3],sys_arg[4],sys_arg[5],ret);
				//fprintf(trace,"syscall:%ld\n",ret);
		}
}


static BOOL INTERCEPTSIGNALCALLBACK(THREADID tid, INT32 sig, CONTEXT *ctxt, BOOL hasHandler, const EXCEPTION_INFO *pExceptInfo, VOID *v)
{
			
			fprintf(trace, "signal:%d\n", sig);
						return true;
						
}

VOID Instruction(INS ins, VOID *v)
{

		     INT32 signal;
				
							   for(signal =1; signal<32 ; signal++){
														
									     PIN_InterceptSignal (signal,INTERCEPTSIGNALCALLBACK,0);
																
									   }


		if(INS_IsSyscall(ins)) {
				
				// Arguments and syscall number is only available before
				INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SysBefore),
								IARG_INST_PTR, IARG_SYSCALL_NUMBER,
								IARG_SYSARG_VALUE, 0, IARG_SYSARG_VALUE, 1,
								IARG_SYSARG_VALUE, 2, IARG_SYSARG_VALUE, 3,
								IARG_SYSARG_VALUE, 4, IARG_SYSARG_VALUE, 5,
								IARG_CONTEXT, IARG_ADDRINT, INS_NextAddress(ins), IARG_END);
		}
				

}


VOID Image(IMG img, VOID *v)
{
		RTN mainRtn = RTN_FindByName(img, "main");
		if(RTN_Valid(mainRtn))
		{
				RTN_Open(mainRtn);
				RTN_InsertCall(mainRtn, IPOINT_BEFORE, (AFUNPTR)MainBegin, IARG_END);
				RTN_InsertCall(mainRtn, IPOINT_AFTER, (AFUNPTR)MainReturn, IARG_END);
				RTN_Close(mainRtn);

				/*if(KnobReplay){
						CheckforSignal();
				}*/
		}

		
		RTN randRtn = RTN_FindByName(img, "rand");

		if(RTN_Valid(randRtn))
		{	
				if(KnobReplay)
				{	
						
						RTN_Replace(randRtn,AFUNPTR(MyRandFunc));					
				}
				else
				{
				RTN_Open(randRtn);
				RTN_InsertCall(randRtn, IPOINT_AFTER, (AFUNPTR)RandReturn, IARG_G_RESULT0, IARG_END);
				RTN_Close(randRtn);
				}
		
		}

}

VOID SyscallExit(THREADID threadIndex, CONTEXT *ctxt, SYSCALL_STANDARD std, VOID *v)
{
		 				SysAfter(PIN_GetSyscallReturn(ctxt, std), PIN_GetSyscallErrno(ctxt, std));
}

VOID Fini(INT32 code, VOID *v)
{
		fclose(trace);
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char *argv[])
{
				PIN_InitSymbols();

    if (PIN_Init(argc, argv)) return Usage();

    if (KnobReplay) {
						printf("====== REPLAY MODE =======\n");
						trace = fopen("trace.out", "r");
						/*if(!feof(trace))
						{
								fgets(newbuffer,250,trace);
								if(strstr(newbuffer,"rand:"))
									{
											IMG_AddInstrumentFunction(Image, 0);
										}
										if(strstr(newbuffer,"syscall:"))
										{
												INS_AddInstrumentFunction(Instruction, 0);
												PIN_AddSyscallExitFunction(SyscallExit, 0);
										}
										else
												INS_AddInstrumentFunction(Instruction, 0);
							}*/
				}
						
				 else {
						printf("====== RECORDING MODE =======\n");
						trace = fopen("trace.out", "w");
		//						 IMG_AddInstrumentFunction(Image, 0);
//			INS_AddInstrumentFunction(Instruction, 0);
							//PIN_AddSyscallExitFunction(SyscallExit, 0);
							 
		//					PIN_AddFiniFunction(Fini, 0);

				}
				
				if(trace == NULL) {
						fprintf(stderr, "File open error! (trace.out)\n");
						return 0;
				}

				IMG_AddInstrumentFunction(Image, 0);
    INS_AddInstrumentFunction(Instruction, 0);
				PIN_AddSyscallExitFunction(SyscallExit, 0);

    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}
