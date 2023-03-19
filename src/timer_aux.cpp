#include "timer_aux.h"

// Incrementa/decrementa um contador usando o delta de tempo,
// sem permitir que saia dos limites máximos/mínimos
void incrementTimer(float& timer, float delta, float timer_max)
{
    if (timer < timer_max)
    {
        timer += delta;

        if (timer > timer_max)
            timer = timer_max;
    }
}

void decrementTimer(float& timer, float delta, float timer_min)
{
    if (timer > timer_min)
    {
        timer -= delta;

        if (timer < timer_min)
            timer = timer_min;
    }
}
