/*#include <dcc_std.h>
#include <dcc_time.h>*/
#include <stdio.h>

#include <time.h>

extern time_t timezone;

int main()
{

  tzsetwall();
  printf("%ld\n",timezone);

  exit(0);

}
