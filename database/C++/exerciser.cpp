#include "exerciser.h"

void exercise(connection *C)
{
    query1(C,1,35,40,0,0,0,0,0,0,0,0,0,1,3,4,0,0,0);
    query2(C,"Green");
    query3(C,"UNC");
    query4(C,"FL","Green");
    query5(C,12);
}
