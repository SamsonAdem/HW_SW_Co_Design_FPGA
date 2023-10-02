/*
 * DMA.cpp
 *
 *  Created on: Apr 20, 2023
 *      Author: samso
 */

#include"dma.hpp"
#include<iostream>

// This code will initialize the DMA through the CPU and the DMA will do the reading and 
// writing, and we see the work of the DMA while communicating with the hardware accelerator
int main() {

    unsigned long int status;

    DirectMemoryAccess* dma = new DirectMemoryAccess(0x40400000, 0x0e000000, 0x0f000000);

 

    dma->reset();

    dma->halt();

 //  In this section we store the data in the DRAM

    dma->writeSourceByte(0);

    dma->writeSourceByte(10);

    dma->writeSourceByte(5);

    dma->writeSourceByte(0);

    dma->writeSourceString("hello");

    dma->hexdumpSource(9); // the byte length: four numerical values plus five char ( 9 bytes)

 // ------------------------------------------------

 // In this section we set the interrupt where it will inform us IOC ( interrupt on complete)
 // we disabled the error event and also the treshold is zero where there is no counting done 
 // by the internal counter.

    dma->setInterrupt(true, false, 0);

    dma->ready();  // ready the DMA

 

    dma->setDestinationAddress(0x0f000000);

    dma->setSourceAddress(0x0e000000);

    dma->setDestinationLength(5); // as the output from the hw accelrator is rovvy( 5 bytes)

    dma->setSourceLength(9);

 
 // In this section  we will wait the DMA till it is done with the task

    printf("Waiting for MM2S...\n");

    do {

        status = dma->getMM2SStatus();

        dma->dumpStatus(status);

    } while((!(status & 1<<12) && !(status & 1 << 1))); // this is the byte counter where we 
	// will check the IOC value at the 13th bit ( count start from 0) is not done and print 
	// status values. The other one also check if the reserved bit is 1. Actually
	// according to the DMA doc this value is always 1. 

 

    printf("Waiting for S2MM...\n");

    do {

        status = dma->getS2MMStatus();

        dma->dumpStatus(status);

    } while((!(status & 1<<12) && !(status & 1 << 1)));

 

    dma->hexdumpDestination(5);
	// Basically dumping is just printing the values of the memory content.

}


