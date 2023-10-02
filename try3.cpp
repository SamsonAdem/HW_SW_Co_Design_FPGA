#include <iostream>
#include <vector>
#include <algorithm>
#define cimg_use_jpeg
#include "CImg.h"

using namespace cimg_library;

std::vector<std::vector<uint8_t>> getPixelValues(const char* filename, int& width, int& height) {
    CImg<uint8_t> image(filename);
    width = image.width();
    height = image.height();

    std::vector<std::vector<uint8_t>> pixelValues(height, std::vector<uint8_t>(width));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            pixelValues[y][x] = image(x, y);
        }
    }

    return pixelValues;
}

std::vector<std::vector<uint8_t>> medianFilter(const std::vector<std::vector<uint8_t>>& image, int filterSize) {
    int width = image[0].size();
    int height = image.size();

    std::vector<std::vector<uint8_t>> filteredImage(height, std::vector<uint8_t>(width));

    int filterOffset = filterSize / 2;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            std::vector<uint8_t> pixels;
            for (int filterY = -filterOffset; filterY <= filterOffset; filterY++) {
                for (int filterX = -filterOffset; filterX <= filterOffset; filterX++) {
                    int imageX = std::min(std::max(x + filterX, 0), width - 1);
                    int imageY = std::min(std::max(y + filterY, 0), height - 1);
                    pixels.push_back(image[imageY][imageX]);
                }
            }

            std::sort(pixels.begin(), pixels.end());
            uint8_t medianValue = pixels[pixels.size() / 2];
            filteredImage[y][x] = medianValue;
        }
    }

    return filteredImage;
}

void saveImage(const std::vector<std::vector<uint8_t>>& image, const char* filename, int width, int height) {
    CImg<uint8_t> outputImage(width, height, 1, 1);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            outputImage(x, y) = image[y][x];
        }
    }

    outputImage.save_jpeg(filename);
}

int main() {
    const char* filename = "lenna4.jpg";
    int width, height;

    // Get the pixel values for the image
    std::vector<std::vector<uint8_t>> pixelValues = getPixelValues(filename, width, height);

    int radius = 3;

    // Apply median filter to the image
    std::vector<std::vector<uint8_t>> filteredImage = medianFilter(pixelValues, radius);

    // Save the filtered image
    const char* outputFilename = "filtered_lenna.jpg";
    saveImage(filteredImage, outputFilename, width, height);

    std::cout << "Median filter applied and filtered image saved successfully." << std::endl;

    return 0;
}
