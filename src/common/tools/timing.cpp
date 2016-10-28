#include "tools/timing.h"

using namespace std;

timespec operator-(const timespec& t2, const timespec& t1) {
	
  timespec result;

  result.tv_sec  = t2.tv_sec  - t1.tv_sec;
  result.tv_nsec = t2.tv_nsec - t1.tv_nsec;

  if(result.tv_nsec < 0) {
    result.tv_sec  -= 1;
    result.tv_nsec += 1000000000;
  }

  return result;
}

timespec operator+(const timespec& t0, const timespec& dt) {
	
  timespec result;

  result.tv_sec  = t0.tv_sec  + dt.tv_sec;
  result.tv_nsec = t0.tv_nsec + dt.tv_nsec;

  if(result.tv_nsec >= 1000000000) {
    result.tv_nsec -= 1000000000;
    result.tv_sec += 1;
  }

  return result;
}

ostream& operator<<(ostream& out, const timespec& t) {
  out << t.tv_sec << ":" << (t.tv_nsec*1.0)/1000000000 << " s";
  return out;
}
