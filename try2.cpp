#include <iostream>
#include <vector>
#include <algorithm>
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb-master/stb_image.h"
#include "lodepng-master/lodepng-master/lodepng.h"
#include"dma.hpp"

std::vector<std::vector<uint8_t>> getPixelValues(const char* filename, int& width, int& height) {
    int channels;

    // Load the image file
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 1);

    // Check if the image was successfully loaded
    if (image == nullptr) {
        std::cout << "Failed to read the image file." << std::endl;
        return std::vector<std::vector<uint8_t>>();
    }

    // Create a 2D vector to store the pixel values
    std::vector<std::vector<uint8_t>> pixelValues(height, std::vector<uint8_t>(width));

    // Copy the pixel values to the 2D vector
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            pixelValues[i][j] = image[i * width + j];
        }
    }

    stbi_image_free(image);

    return pixelValues;
}
////////////////////////////////////// HARDWARE ACCELERATOR /////////////////////////////////////////////////


// std::vector<std::vector<uint8_t>> medianFilter(const std::vector<std::vector<uint8_t>>& image, int filterSize) {
//     int width = image[0].size();
//     int height = image.size();

//     std::vector<std::vector<uint8_t>> filteredImage(height, std::vector<uint8_t>(width));

//     int filterOffset = filterSize / 2;

//     for (int y = 0; y < height; y++) {
//         for (int x = 0; x < width; x++) {
//             std::vector<uint8_t> pixels;
//             for (int filterY = -filterOffset; filterY <= filterOffset; filterY++) {
//                 for (int filterX = -filterOffset; filterX <= filterOffset; filterX++) {
//                     int imageX = std::min(std::max(x + filterX, 0), width - 1);
//                     int imageY = std::min(std::max(y + filterY, 0), height - 1);
//                     pixels.push_back(image[imageY][imageX]);
//                 }
//             }

//             std::sort(pixels.begin(), pixels.end());
//             uint8_t medianValue = pixels[pixels.size() / 2];
//             filteredImage[y][x] = medianValue;
//         }
//     }

//     return filteredImage;
// }


//////////////////////////////////////////////////////////////////////////////////////////////////


void saveImage(const std::vector<std::vector<uint8_t>>& image, const std::string& filename) {
    int width = image[0].size();
    int height = image.size();
    std::vector<unsigned char> buffer(width * height);

    // Copy the pixel values to the buffer
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            buffer[y * width + x] = image[y][x];
        }
    }

    // Save the image in PNG format
    if (lodepng::encode(filename, buffer, width, height, LCT_GREY, 8) != 0) {
        std::cout << "Failed to save the image file." << std::endl;
    }
}

int main() {
    const char* filename = "lenna4.jpg";
    int width, height;

    // Get the pixel values for the image
    std::vector<std::vector<uint8_t>> pixelValues = getPixelValues(filename, width, height);

    int radius = 7;

    // Apply median filter to the image
    // std::vector<std::vector<uint8_t>> filteredImage = medianFilter(pixelValues, radius);

    unsigned long int status;

    DirectMemoryAccess* dma = new DirectMemoryAccess(0x40400000, 0x0e000000, 0x0f000000);

 

    dma->reset();

    dma->halt();

    size_t rows=512, cols=512;

    dma->write2DArray(0x0e000000,pixelValues, rows, cols);

    dma->hexdumpSource(256*512);
    

    dma->setInterrupt(true, false, 0);

    dma->ready();  // ready the DMA



    dma->setDestinationAddress(0x0f000000);

    dma->setSourceAddress(0x0e000000);

    dma->setDestinationLength(256*512);

    dma->setSourceLength(256*512);




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



    dma->hexdumpDestination(256*512);

    // Save the filtered image
    std::string outputFilename = "lenna4_filtered.png";
    saveImage(filteredImage, outputFilename);

    std::cout << "Median filter applied and filtered image saved successfully." <<std::endl;

    return 0;
}





































