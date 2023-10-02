#include <iostream>
#include <cstring>
#include "dma.hpp"

int main() {
    DirectMemoryAccess* dma = new DirectMemoryAccess(0x40400000, 0x0e000000, 0x0f000000);

    // Data to be processed
    int input_data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int output_data[10];

    // Reset DMA and prepare for transfer
    dma->reset();
    dma->halt();
    dma->ready();

    // Set source and destination addresses
    dma->setSourceAddress(0x0e000000);
    dma->setDestinationAddress(0x0f000000);

    // Set transfer lengths
    dma->setSourceLength(10 * sizeof(int));
    dma->setDestinationLength(10 * sizeof(int));

    // Copy input data to the source address
    memcpy((void*)dma->getSourceAddress(), input_data, 10 * sizeof(int));

    // Start the DMA transfer
    dma->setInterrupt(true, false, 0);

    // Wait for MM2S transfer to complete
    unsigned long int status;
    do {
        status = dma->getMM2SStatus();
    } while (!(status & (1 << 12)) && !(status & (1 << 1)));

    // Wait for S2MM transfer to complete
    do {
        status = dma->getS2MMStatus();
    } while (!(status & (1 << 12)) && !(status & (1 << 1)));

    // Copy processed data from the destination address
    memcpy(output_data, (void*)dma->getDestinationAddress(), 10 * sizeof(int));

    // Print the processed data
    for (int i = 0; i < 10; i++) {
        std::cout << "Output[" << i << "]: " << output_data[i] << std::endl;
    }

    delete dma;
    return 0;
}
