/*
 * DMA.cpp
 *
 *  Created on: Apr 20, 2023
 *      Author: samso
 */

#include"dma.hpp"

#include<iostream>


int main() {

    unsigned long int status;

    DirectMemoryAccess* dma = new DirectMemoryAccess(0x40400000, 0x0e000000, 0x0f000000);

 

    dma->reset();

    dma->halt();



    // dma->write2DArray()

    dma->hexdumpSource(8); 


    dma->setInterrupt(true, false, 0);

    dma->ready();  // ready the DMA



    dma->setDestinationAddress(0x0f000000);

    dma->setSourceAddress(0x0e000000);

    dma->setDestinationLength(8);

    dma->setSourceLength(8);




    printf("Waiting for MM2S...\n");

    do {

        status = dma->getMM2SStatus();

        dma->dumpStatus(status);

    } while((!(status & 1<<12) && !(status & 1 << 1))); 


    printf("Waiting for S2MM...\n");

    do {

        status = dma->getS2MMStatus();

        dma->dumpStatus(status);

    } while((!(status & 1<<12) && !(status & 1 << 1)));



    dma->hexdumpDestination(8);
    


}



