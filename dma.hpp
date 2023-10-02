#ifndef DMA_HPP
#define DMA_HPP

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <cstdint>
#define MM2S_CONTROL_REGISTER 0x00
#define MM2S_STATUS_REGISTER 0x04
#define MM2S_SOURCE_ADDRESS 0x18
#define MM2S_LENGTH 0x28

#define S2MM_CONTROL_REGISTER 0x30
#define S2MM_STATUS_REGISTER 0x34
#define S2MM_DESTINATION_ADDRESS 0x48
#define S2MM_LENGTH 0x58
typedef struct ap_axi {
    uint32_t data;   // Represents data you wrote on the DRAM
    uint8_t keep;   // Used when words are longer than 32 bits
    bool last;   // Assert the last data of the burst
} ap_axi;
class DirectMemoryAccess
{
private:
    unsigned int offset;
    unsigned int mm2s_status, s2mm_status;
    unsigned int *addr, *src_addr, *dst_addr;

    void hexdump(void *addr, unsigned int len)
    {
        int i;
        unsigned char buff[17];
        unsigned char *pc = (unsigned char *)addr;

        for (i = 0; i < len; i++)
        {
            if ((i % 16) == 0)
            {
                if (i != 0)
                    printf("  %s\n", buff);
                printf("%04x:", i);
            }

            printf(" %02x", pc[i]);

            if ((pc[i] < 0x20) || (pc[i] > 0x7e))
                buff[i % 16] = '.';
            else
                buff[i % 16] = pc[i];
            buff[(i % 16) + 1] = '\0';
        }

        while ((i % 16) != 0)
        {
            printf("   ");
            i++;
        }

        printf("  %s\n", buff);
    }

public:
    DirectMemoryAccess(unsigned int addr, unsigned int src_addr, unsigned int dst_addr) : offset(0)
    {
        int dh = open("/dev/mem", O_RDWR | O_SYNC);                                                           // Open /dev/mem which represents the whole physical memory
        this->addr = (unsigned int *)mmap(NULL, 65535, PROT_READ | PROT_WRITE, MAP_SHARED, dh, addr);         // Memory map AXI Lite register block
        this->src_addr = (unsigned int *)mmap(NULL, 65535, PROT_READ | PROT_WRITE, MAP_SHARED, dh, src_addr); // Memory map source address
        this->dst_addr = (unsigned int *)mmap(NULL, 65535, PROT_READ | PROT_WRITE, MAP_SHARED, dh, dst_addr); // Memory map destination address
    }

    void ready()
    {
        this->addr[S2MM_CONTROL_REGISTER >> 2] |= (1u << 0);
        this->addr[MM2S_CONTROL_REGISTER >> 2] |= (1u << 0);
    }
    void halt()
    {
        this->addr[S2MM_CONTROL_REGISTER >> 2] &= ~(1u << 0);
        this->addr[MM2S_CONTROL_REGISTER >> 2] &= ~(1u << 0);
    }
    void reset()
    {
        this->addr[S2MM_CONTROL_REGISTER >> 2] |= (1u << 2);
        this->addr[MM2S_CONTROL_REGISTER >> 2] |= (1u << 2);
    }

    void setSourceLength(unsigned int length)
    {
        this->addr[MM2S_LENGTH >> 2] = length;
    }
    void setDestinationLength(unsigned int length)
    {
        this->addr[S2MM_LENGTH >> 2] = length;
    }

    void setInterrupt(bool enable_complete, bool enable_error, unsigned char threshold)
    {
        this->addr[S2MM_CONTROL_REGISTER >> 2] &= 0x00000fff;                                                                                                        // Clear
        this->addr[S2MM_CONTROL_REGISTER >> 2] |= /*(delay << 24) +*/ (threshold << 16) + (enable_error << 14) + /*(enable_delay << 13) +*/ (enable_complete << 12); // Set

        this->addr[MM2S_CONTROL_REGISTER >> 2] &= 0x00000fff;                                                                                                        // Clear
        this->addr[MM2S_CONTROL_REGISTER >> 2] |= /*(delay << 24) +*/ (threshold << 16) + (enable_error << 14) + /*(enable_delay << 13) +*/ (enable_complete << 12); // Set
    }

    unsigned int getSourceAddress() {
        return this->addr[MM2S_SOURCE_ADDRESS >> 2];
    }
    unsigned int getDestinationAddress() {
        return this->addr[S2MM_DESTINATION_ADDRESS >> 2];
    }
    void setSourceAddress(unsigned int addr)
    {
        this->addr[MM2S_SOURCE_ADDRESS >> 2] = addr;
    }
    void setDestinationAddress(unsigned int addr)
    {
        this->addr[S2MM_DESTINATION_ADDRESS >> 2] = addr;
    }

    unsigned int getMM2SStatus()
    {
        return this->addr[MM2S_STATUS_REGISTER >> 2];
    }
    unsigned int getS2MMStatus()
    {
        return this->addr[S2MM_STATUS_REGISTER >> 2];
    }

    void hexdumpSource(unsigned int length)
    {
        this->hexdump(this->src_addr, length);
    }
    void hexdumpDestination(unsigned int length)
    {
        this->hexdump(this->dst_addr, length);
    }
    void dumpStatus(unsigned int status)
    {
        printf("Status:");
        if (status & 0x00000001)
            printf(" halted");
        else
            printf(" running");
        if (status & 0x00000002)
            printf(" idle");
        if (status & 0x00000008)
            printf(" SGIncld");
        if (status & 0x00000010)
            printf(" DMAIntErr");
        if (status & 0x00000020)
            printf(" DMASlvErr");
        if (status & 0x00000040)
            printf(" DMADecErr");
        if (status & 0x00000100)
            printf(" SGIntErr");
        if (status & 0x00000200)
            printf(" SGSlvErr");
        if (status & 0x00000400)
            printf(" SGDecErr");
        if (status & 0x00001000)
            printf(" IOC_Irq");
        if (status & 0x00002000)
            printf(" Dly_Irq");
        if (status & 0x00004000)
            printf(" Err_Irq");
        printf("\n");
    }

    void writeSourceByte(char c)
    {
        char *p = (char *)this->src_addr;
        p[this->offset] = c;
        this->offset++;
    }
    void writeSourceString(const char *str)
    {
        char *p = (char *)this->src_addr;
        unsigned int i, length = strlen(str);
        for (i = 0; i < length; i++)
        {
            p[this->offset + i] = str[i];
        }
        this->offset += length;
    }
    void writeSourceInteger(unsigned int i)
    {
        this->src_addr[this->offset/4 ] = i;
        this->offset += 4;
    }
    
    void write2DArray(unsigned int mem, std::vector<std::vector<uint8_t>> pixelValues1, size_t rows, size_t cols)
    {
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            unsigned int val = pixelValues1[i][j];
            writeSourceInteger(val);
        }
    }
    }


    void resetCursor()
    {
        this->offset = 0;
    }
    void writeSourceApAxI(int32_t data, uint8_t keep, bool last) {
    ap_axi value;
    value.data = data;
    value.keep = keep;
    value.last = last;

    // Assuming this->src_addr is an array of ap_axi
    this->src_addr[this->offset / 4] = data;
    this->offset += 4;
    this->src_addr[this->offset / 2] = keep;
    this->offset += 4;
     this->src_addr[this->offset ] = last;
    this->offset += 4;
    }
};

#endif