#include <stdio.h>
#include <stdint.h>

// Memory-mapped address of the Zedboard's memory location
#define MEMORY_BASE_ADDRESS 0x00000000

// Size of the array
#define ARRAY_SIZE 10

int main() {
    // Define the array
    uint32_t array[ARRAY_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Set up the communication interface with the Zedboard
    // (e.g., establish a connection via Ethernet, USB, or UART)

    // Transfer the array to the Zedboard's memory
    uint32_t *memory_ptr = (uint32_t *)MEMORY_BASE_ADDRESS;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        *(memory_ptr + i) = array[i];
    }

    // Verify the array has been written to memory
    for (int i = 0; i < ARRAY_SIZE; i++) {
        uint32_t value = *(memory_ptr + i);
        printf("Memory address: 0x%X, Value: %u\n", (MEMORY_BASE_ADDRESS + i * sizeof(uint32_t)), value);
    }

    // Close the communication interface

    return 0;
}
