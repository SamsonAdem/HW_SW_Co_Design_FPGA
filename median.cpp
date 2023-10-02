#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "dma.hpp"

#define INPUT_IMAGE_ADDR 0x0e000000  // Define the input image address
#define OUTPUT_IMAGE_ADDR 0x0f000000 // Define the output image address
#define IMAGE_SIZE        512 * 512

void loadInputImage(uint8_t* inputImage)
{
    // TODO: Implement your own image loading logic here
    // You need to populate the inputImage buffer with the input image data

    // Example: Loading a grayscale image from a file
    FILE* inputFile = fopen("input_image.bin", "rb");
    if (inputFile == NULL)
    {
        printf("Failed to open input image file.\n");
        return;
    }

    size_t bytesRead = fread(inputImage, sizeof(uint8_t), IMAGE_SIZE, inputFile);
    if (bytesRead != IMAGE_SIZE)
    {
        printf("Error reading input image file.\n");
    }

    fclose(inputFile);
}

void saveOutputImage(uint8_t* outputImage)
{
    // TODO: Implement your own logic to save the output image
    // You can save the outputImage buffer as an image file

    // Example: Saving the output image as a grayscale image to a file
    FILE* outputFile = fopen("output_image.bin", "wb");
    if (outputFile == NULL)
    {
        printf("Failed to create output image file.\n");
        return;
    }

    size_t bytesWritten = fwrite(outputImage, sizeof(uint8_t), IMAGE_SIZE, outputFile);
    if (bytesWritten != IMAGE_SIZE)
    {
        printf("Error writing output image file.\n");
    }

    fclose(outputFile);
}

int main()
{
    // Initialize DMA
    DirectMemoryAccess dma(0x40400000, INPUT_IMAGE_ADDR, OUTPUT_IMAGE_ADDR);

    // Allocate memory buffers for input and output images
    uint8_t* inputImage = (uint8_t*)malloc(IMAGE_SIZE);
    uint8_t* outputImage = (uint8_t*)malloc(IMAGE_SIZE);

    // Load input image data into input buffer
    loadInputImage(inputImage);

    // Configure DMA for input transfer
    dma.setSourceAddress(INPUT_IMAGE_ADDR);
    dma.setDestinationAddress(0x0e000000);
    dma.setSourceLength(IMAGE_SIZE);
    dma.ready();

    // Start DMA transfer for input
    dma.halt();
    dma.reset();
    dma.ready();

    // Wait for DMA transfer completion
    while (!(dma.getS2MMStatus() & 0x01))
        ;

    // Configure DMA for output transfer
    dma.setSourceAddress(0x0f000000);
    dma.setDestinationAddress(OUTPUT_IMAGE_ADDR);
    dma.setDestinationLength(IMAGE_SIZE);
    dma.ready();

    // Start DMA transfer for output
    dma.halt();
    dma.reset();
    dma.ready();

    // Wait for DMA transfer completion
    while (!(dma.getMM2SStatus() & 0x01))
        ;

    // Save the output image
    saveOutputImage(outputImage);

    // Free memory buffers
    free(inputImage);
    free(outputImage);

    return 0;
}
