#ifndef _BD_REGION_H_
#define _BD_REGION_H_
#include <stdint.h>

typedef enum bd_region_state_t
{
    BD_REGION_FREE = 0,
    BD_REGION_USED
}
bd_region_state_t;


typedef struct bd_header_t
{
    uint8_t state;
    uint8_t reserved[3];
    uint32_t size;
}
bd_header_t;

struct bd_region_t
{
    bd_header_t header;
    char memory[];
};


#endif/*_BD_REGION_H_*/
