#include "time_utils.h"

uint32_t get_next_diff_time(uint32_t last_time, uint32_t diff_next_time, uint32_t current_time)
{
    uint32_t next_time = last_time + diff_next_time;

    //current_time overflow
    if (current_time < last_time)
    {
        //next_time overflow
        if (next_time < last_time)
        {
            if (next_time < current_time) return 0; //next time is less than current_time
            return next_time - current_time;
        }
        
        //current_time overflow and next_time is not overflow
        // -> next_time is less than current_time
        return 0;
    }

    //if next_time overflow or not overflow and current_time is not
    //-> do diff, it's must be ok
    return next_time - current_time;
}