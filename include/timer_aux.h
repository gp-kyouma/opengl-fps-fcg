#ifndef FCG_TIMER
#define FCG_TIMER

// Incrementa/decrementa um contador usando o delta de tempo,
// sem permitir que saia dos limites máximos/mínimos
void incrementTimer(float& timer, float delta, float timer_max);
void decrementTimer(float& timer, float delta, float timer_min);

#endif // FCG_TIMER
