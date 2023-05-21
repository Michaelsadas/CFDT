//#include <stdint.h>
//#include <stddef.h>
//#include <assert.h>
//#include <stdlib.h>
#include <stdio.h>
//#ifndef BAREMETAL
//#include <sys/mman.h>
//#endif
#include "include/rocc.h"
#include "include/ISA.h"

void cgra_execute(void** din_addr, void** dout_addr)
{
	unsigned short int cin[39][3] __attribute__((aligned(8))) = {
		{0x70f0, 0x6000, 0x0018},
		{0x8400, 0x21ff, 0x0019},
		{0x7080, 0x0101, 0x001a},
		{0x0810, 0x0101, 0x001b},
		{0x60f0, 0x6000, 0x0020},
		{0x8400, 0x21ff, 0x0021},
		{0x7080, 0x0100, 0x0022},
		{0x0010, 0x0000, 0x0023},
		{0x700f, 0x6000, 0x0028},
		{0xc000, 0x21ff, 0x0029},
		{0x0000, 0x0100, 0x002a},
		{0x0010, 0x0000, 0x002b},
		{0x0000, 0x6000, 0x0030},
		{0x0000, 0x2000, 0x0031},
		{0x0300, 0x0100, 0x0032},
		{0x0010, 0x0000, 0x0033},
		{0x0000, 0x0000, 0x0038},
		{0x0018, 0x0000, 0x0039},
		{0x0000, 0x0000, 0x003a},
		{0x0310, 0x0102, 0x003b},
		{0x0000, 0x0000, 0x0068},
		{0x6000, 0x0000, 0x0070},
		{0x8400, 0x0001, 0x0078},
		{0x0000, 0x0000, 0x0080},
		{0x0001, 0x0000, 0x0088},
		{0x9041, 0x0000, 0x00b1},
		{0x8863, 0x0000, 0x00b9},
		{0x4801, 0x0000, 0x00c1},
		{0x0004, 0x0000, 0x00c8},
		{0x1803, 0x0000, 0x00c9},
		{0x0030, 0x0000, 0x0110},
		{0x0000, 0x0000, 0x01a0},
		{0x2003, 0x0001, 0x01e9},
		{0x0000, 0x0000, 0x0238},
		{0xffff, 0xffff, 0x0280},
		{0x000f, 0x001e, 0x0281},
		{0x0000, 0x000c, 0x0282},
		{0x0800, 0x0080, 0x0283},
		{0x0008, 0x0000, 0x0284},
	};

	load_cfg(cin, 0x40000, 234, 0, 0);
	load_data(din_addr[0], 0x10000, 76, 0, 0);
	load_data(din_addr[1], 0x14000, 124, 0, 0);
	load_data(din_addr[2], 0x18000, 1024, 0, 0);
	load_data(din_addr[3], 0x8000, 1024, 0, 0);
	config(0x0, 39, 0, 0);
	execute(0x7c, 0, 0);
	store(dout_addr[0], 0xc000, 7168, 0, 0);
}


#define NI 10
#define NJ 16
#define NK 16
#define ASIZE (3*NI)
#define BSIZE (NJ+NK)
#define CSIZE (NJ*NJ)

//int A[ASIZE] __attribute__((aligned(8)));
//int B[BSIZE] __attribute__((aligned(8)));
//int C[CSIZE] __attribute__((aligned(8)));
//int D[NI][NJ][NK] __attribute__((aligned(8)));
//int D1[NI][NJ][NK] __attribute__((aligned(8)));
//int E[NJ][NK] __attribute__((aligned(8)));

int A[ASIZE];
int B[BSIZE];
int C[CSIZE];
int D[NI][NJ][NK];
int D1[NI][NJ][NK];
int E[NJ][NK];

void test(){
   for (int i=2; i<NI; i=i+2){
      for (int j=NJ-1; j>=0; j--){
   	    for(int k=0; k<NK; k=k+3){
   		    int tem = A[i*3] + B[j+k];
   		    D1[i][j][k] = C[j*j] * E[j][k]+tem;
   		}
   	  }
   }
 }

int main(){
//#ifndef BAREMETAL
//    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
//      perror("mlockall failed");
//      exit(1);
//    }
//#endif

    for (int i=0; i<ASIZE; i++){
      A[i] = i * 2 + 5;
    }
    for (int i=0; i<BSIZE; i++){
      B[i] = i * 3;
    }
    for (int i=0; i<CSIZE; i++){
      C[i] = i;
    }
    for (int j=0; j<NJ; j++){
        for(int k=0; k<NK; k++)
          E[j][k] = k+j;
    }

    printf("%x, %x, %x, %x, %x\n", A, B, C, D, E);

    void* cgra_din_addr[4] = {(void*)A+24, B, C, E};
    void* cgra_dout_addr[1] = {(void*)D+2048};
    cgra_execute(cgra_din_addr, cgra_dout_addr);
    test();

//    printf("Here comes the result: \n");
    for (int i=2; i<NI; i=i+2){
        for(int j = 0; j < NJ; j++)
            for(int k = 0; k < NK; k++){
                if(D[i][j][k] != D1[i][j][k]){
                    printf("%d, %d\n", D[i][j][k], D1[i][j][k]);
                }
            }
    }
    printf("Succeed!\n");

    return 0;
}
