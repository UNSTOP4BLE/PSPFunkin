#include "memory.h"   
#include <cstdlib>            
#include <cstdint>           

namespace Mem {
int extshit(void)
{
    uint32_t size, sizeblock;
    uint8_t *ram;
    // Init variables
    size = 0;
    sizeblock = (1024 * 1024);
    // Check loop
    while (sizeblock)
    {
        // Increment size
        size += sizeblock;
        // Allocate ram
        ram = (uint8_t*)malloc(size);
        // Check allocate
        if (!(ram))
        {
            // Restore old size
            size -= sizeblock;
            // Size block / 2
            sizeblock >>= 1;
        }
        else
            free(ram);
    }
    
    return size;
}

int freememory(void)
{
    uint8_t **ram, **temp;
    uint32_t size, count, x;
    // Init variables
    ram = NULL;
    size = 0;
    count = 0;
    // Check loop
    for (;;)
    {
        // Check size entries
        if (!(count % 10))
        {
            // Allocate more entries if needed
            temp = (uint8_t**)realloc(ram,sizeof(uint8_t*) * (count + 10));
            if (!(temp)) break;
            // Update entries and size (size contains also size of entries)
            ram = temp;
            size += (sizeof(uint8_t *) * 10);
        }
        // Find max lineare size available
        x = extshit();
        if (!(x)) break;
        // Allocate ram
        ram[count] = (uint8_t*)malloc(x);
        if (!(ram[count])) break;
        // Update variables
        size += x;
        count++;
    }
    // Free ram
    if (ram)
    {
        for (x=0;x<count;x++) free(ram[x]);
        free(ram);
    }
    return (int)size;
}
}