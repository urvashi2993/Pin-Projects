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
#include <iostream>
#include <fstream>
#include "pin.H"
#include<map>
FILE* outFile;

bool isMainProgram = false;
ADDRINT highImgAddr = 0;
//std::map<ADDRINT, UINT32> mapForIP;
//mapForIP.insert(std::make_pair("IP",IARG_INST_PTR));

VOID memWrite(THREADID tid, ADDRINT target, UINT32 size, ADDRINT ip, const string filename, UINT32 line)
{
		if(!isMainProgram) return;
		if(target >= highImgAddr) return;
		if(filename.empty()) return;

		fprintf(outFile, "memW: ip: 0x%lx, target: 0x%lx, size: 0x%x - %s:%d\n", ip, target, size, filename.c_str(), line);
}

VOID memRead(THREADID tid, ADDRINT target,UINT32 size, ADDRINT ip, const string filename, UINT32 line)
{
if(!isMainProgram) return;
if(target >= highImgAddr) return;
if(filename.empty()) return;
		
		fprintf(outFile, "memR: ip: 0x%lx,target:0x%lx,  size: 0x%x - %s:%d\n", ip, target, size, filename.c_str(), line);
}

VOID regRead(THREADID tid, const string reg, ADDRINT ip, const string filename, UINT32 line)
  {
			  if(!isMainProgram) return;
				// if(target >= highImgAddr) return;
					  if(filename.empty()) return;
			fprintf(outFile, "RegR: ip: 0x%lx, reg:%s, regName: - %s:%d\n",ip,reg.c_str(),  filename.c_str(), line);		 
	// fprintf(outFile,"0x\n",reg.str());
		
		//fprintf(outFile,"Reg R", ip, eax, filename.c_str());
								  }
VOID regWrite(THREADID tid, const string reg, ADDRINT ip, const string filename, UINT32 line)
    {
		      if(!isMainProgram) return;
				      // if(target >= highImgAddr) return;
					         if(filename.empty()) return;
					     fprintf(outFile, "RegW: ip: 0x%lx, reg:%s, regName: - %s:%d\n",ip,reg.c_str(),  filename.c_str(), line);
							        }

VOID MainBegin()
{
		isMainProgram = true;
}

VOID MainReturn()
{
		isMainProgram = false;
}

bool IsAddressInMainExecutable(ADDRINT addr)
{
		RTN rtn = RTN_FindByAddress(addr);

		if (rtn == RTN_Invalid())
			return false;

		SEC sec = RTN_Sec(rtn);
		if (sec == SEC_Invalid())
				return false;

		IMG img = SEC_Img(sec);
		if (img == IMG_Invalid())
				return false;
		if(IMG_IsMainExecutable(img)) return true;

		return false;
}

VOID Image(IMG img, VOID *v){

		if(highImgAddr < IMG_HighAddress(img)) highImgAddr = IMG_HighAddress(img);
		
		RTN mainRtn = RTN_FindByName(img, "main");
		
		if(RTN_Valid(mainRtn))
		{
				RTN_Open(mainRtn);
				RTN_InsertCall(mainRtn, IPOINT_BEFORE, (AFUNPTR)MainBegin, IARG_END);
				RTN_InsertCall(mainRtn, IPOINT_AFTER, (AFUNPTR)MainReturn, IARG_END);
				RTN_Close(mainRtn);
		}
}

VOID Instruction(INS ins, VOID *v)
{
		if (!IsAddressInMainExecutable(INS_Address(ins))) return;
  
		string filename;
		INT32 line;
		UINT32 count;
		REG r1, r2;


PIN_GetSourceLocation(INS_Address(ins), NULL, &line, &filename);

//mapForIP.insert(std::make_pair(IARG_INST_PTR,EP));
//mapForIP.insert(std::make_pair("LIP", NULL));
//std::map<std::string, int>::iterator it = mapOfWords.begin();

	/*	if(INS_IsMemoryRead(ins)) {
		if(it->first==IARG_INST_PTR){
			 it->second++;
			}*/
if(INS_IsMemoryRead(ins)){
	//mapForIP.insert(std::make_pair(IARG_INST_PTR,EP));
	INS_InsertCall(
								ins, IPOINT_BEFORE, (AFUNPTR) memRead,
								IARG_THREAD_ID,
								IARG_MEMORYREAD_EA,
								IARG_MEMORYREAD_SIZE,
								IARG_INST_PTR,
								IARG_PTR, new string(filename),
								IARG_UINT32, line,
								IARG_END);
				

		}
		
	/*	if(mapForIP["IP"]==){
				 EP++;
		 }*/
//		if(mapForIP["IP"]==IARG_INST_PTR)
		if(INS_IsMemoryWrite(ins)) {
				INS_InsertCall(
								ins, IPOINT_BEFORE, (AFUNPTR) memWrite,
								IARG_THREAD_ID,
								IARG_MEMORYWRITE_EA,
								IARG_MEMORYWRITE_SIZE,
								IARG_INST_PTR,
								IARG_PTR, new string(filename),
								IARG_UINT32, line,
								IARG_END);
		}
		count =0;
		while(count<=INS_MaxNumRRegs(ins)){
		r1=INS_RegW(ins,count);

		if(INS_RegWContain(ins,r1)){
INS_InsertCall	(	
ins, IPOINT_BEFORE, (AFUNPTR) regWrite,
         IARG_THREAD_ID,
									IARG_PTR, new string(REG_StringShort (r1)),
         IARG_INST_PTR,
         IARG_PTR, new string(filename),
         IARG_UINT32, line,
         IARG_END);

		}
r2=INS_RegR(ins,count);
 
  if(INS_RegRContain(ins,r2)){
      INS_InsertCall (
 ins, IPOINT_BEFORE, (AFUNPTR) regRead,
          IARG_THREAD_ID,
          IARG_PTR, new string(REG_StringShort (r2)),
          IARG_INST_PTR,
          IARG_PTR, new string(filename),
          IARG_UINT32, line,
		          IARG_END);
}
		count++;
}
}
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
				"o", "Project2.out","specify output file name");

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
		fclose(outFile);
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
		cerr << "This tool capture data dependence edges and generate a trace." << endl;
		cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
		return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
/*   argc, argv are the entire command line: pin -t <toolname> -- ...    */
/* ===================================================================== */

int main(int argc, char * argv[])
{
		PIN_InitSymbols();
		// Initialize pin
	//	outFile=fopen("project2.out","w");
 outFile = fopen(KnobOutputFile.Value().c_str(), "w");
if (PIN_Init(argc, argv)) return Usage();

		//outFile = fopen(KnobOutputFile.Value().c_str(), "w");
		fprintf(outFile, "start writing \n");
		if(outFile == NULL) {
				fprintf(stderr, "Fail to open the output file - %s\n", KnobOutputFile.Value().c_str());
				return 0;
		}

		IMG_AddInstrumentFunction(Image, 0);
		// Register Instruction to be called to instrument instructions
		INS_AddInstrumentFunction(Instruction, 0);

		// Register Fini to be called when the application exits
		PIN_AddFiniFunction(Fini, 0);

		// Start the program, never returns
		PIN_StartProgram();

		return 0;
}
