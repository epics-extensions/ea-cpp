
#ifndef EA4_TIMING_H
#define EA4_TIMING_H

#include <time.h>
#include <iostream>


timespec operator-(const timespec& t2, const timespec& t1);
timespec operator+(const timespec& t0, const timespec& dt);

std::ostream& operator<<(std::ostream& out, const timespec& t);


#endif






