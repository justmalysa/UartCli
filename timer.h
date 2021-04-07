#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>

#define ARR_SIZE    20

/* Function prototypes ------------------------------------------------------- */

/** Initializes the TIM interface.
 *  @return true if the initialization was successfull and the port is operational,
 *          false otherwise
 */
bool TIM_Init(void);

#endif /* TIMER_H */
