#include <time.h>                // for time()
#include <stdlib.h>              // for rand/srand
#include "randgen.h"
//#include &lt;cmath&gt;                 // *** necessary?

int RandGen::ourInitialized = 0;

// constructors

RandGen::RandGen()
// postcondition: system srand() used to initialize seed
//                once per program     
{
  if (0 == ourInitialized)
  {
    ourInitialized = 1;      // only call srand once
    srand(unsigned(time(0)));       // randomize
  }
}

RandGen::RandGen(int seed)
// postcondition: system srand() used to initialize seed
//                once per program     
{
  if (0 == ourInitialized)
  {
    ourInitialized = 1;      // only call srand once
    srand(seed);             // randomize
  }
}

// public accessing functions

int RandGen::RandInt(int max)
// precondition: max &gt; 0
// postcondition: returns int in [0..max)     
{  
  return int(RandReal() * max);
}

int RandGen::RandInt(int min, int max)
// precondition: min &lt;= max     
// postcondition: returns int in [min..max]     
{ 
  return min + RandInt(max - min + 1);
}

double RandGen::RandReal()
// postcondition: returns double in [0..1)     
{     
  // RAND_MAX is defined in &lt;stdlib.h&gt;
  return rand() / (double(RAND_MAX) + 1); 
}

/* *** Old version: why inconsistent with RandInt?
double RandGen::RandReal(double low, double high)
{
  double width = fabs(high-low);
  double thelow = low &lt; high ? low : high;
  return (RandReal() * width) + thelow;
}
*/

double RandGen::RandReal(double dmin, double dmax)
// precondition: dmin &lt;= dmax     
// postcondition: returns double in [dmin..dmax)     
{
  double width = dmax - dmin;
  return RandReal() * width + dmin;
}
