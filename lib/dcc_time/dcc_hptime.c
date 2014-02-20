#ifndef HPLB_DEFINED
#define HPLB_DEFINED

struct __hptime {
  long julian;
  long secs;
  long nano;
};

typedef struct __hptime HPTIME;


#endif

