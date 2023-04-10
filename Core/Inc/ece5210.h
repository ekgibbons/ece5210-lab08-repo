#ifndef ECE5210_H
#define ECE5210_H

#include <stdint.h>

/* 
   this controls the right channel behavior.  if defined,
   this will just pass the original signal through the right
   channel.  if not defined, it will copy the processed samples
   into the right channel.
*/
#define PASSTHROUGH_RIGHT

void init_firwin(void);
int16_t process_sample_left(int16_t sample_in);
int16_t process_sample_right(int16_t sample_in);

#endif
