#include "Randomizer.h"

#include <iostream>
#include <time.h>
#include <stdlib.h>

using namespace std;

Randomizer* Randomizer::s_pInstance = 0;

void Randomizer::reseed ()
{
    // reseed random generator
    time_t now;
    time(&now);
    srand(now);
    rand();
}

int Randomizer::getRandomInt()
{
    return int(rand());
}

Randomizer::Randomizer()
{
    reseed();
}
