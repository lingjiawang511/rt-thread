#ifndef __FILTER_H_
#define __FILTER_H_

//#include "stm32f4xx_hal.h"
#include "include/myType.h"

void InitFilter(long dat);
void Filter(long Newdat);
long filter_1st_const(long newdata, long olddata, long filter, u8 k);
long filter_1st_var(long newdata, long olddata);

#endif


