/* 
 * Copyright (C) 2008 Mark Hills <mark@pogo.org.uk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2, as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * version 2 along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

#ifndef TIMECODER_H
#define TIMECODER_H

#include "device.h"
#include "pitch.h"

#define TIMECODER_CHANNELS DEVICE_CHANNELS


typedef unsigned int bits_t;


struct timecode_def_t {
    char *name, *desc;
    int bits, /* number of bits in string */
        resolution, /* wave cycles per second */
        flags;
    bits_t seed, /* LFSR value at timecode zero */
        taps; /* central LFSR taps, excluding end taps */
    unsigned int length, /* in cycles */
        safe; /* last 'safe' timecode number (for auto disconnect) */
    signed int *lookup; /* pointer to built lookup table */
};


struct timecoder_channel_t {
    int positive, /* wave is in positive part of cycle */
	swapped; /* wave recently swapped polarity */
    signed int zero;
    int crossing_ticker; /* samples since we last crossed zero */
};


struct timecoder_t {
    struct timecode_def_t *def;
    int forwards, rate;

    /* Signal levels */

    signed int signal_level, ref_level;
    struct timecoder_channel_t primary, secondary;

    /* Filter precalculations */

    float zero_alpha, signal_alpha;

    /* Pitch information */

    struct pitch_t pitch;
    unsigned int crossing_ticker; /* time in samples since last observation */

    /* Numerical timecode */

    bits_t bitstream, /* actual bits from the record */
        timecode; /* corrected timecode */
    int valid_counter, /* number of successful error checks */
        timecode_ticker; /* samples since valid timecode was read */

    /* Feedback */

    unsigned char *mon; /* x-y array */
    int mon_size, mon_counter,
        log_fd; /* optional file descriptor to log to, or -1 for none */
};


/* Building the lookup table is global. Need a good way to share
 * lookup tables soon, so we can use a different timecode on 
 * each timecoder, and switch between them. */

int timecoder_build_lookup(char *timecode_name);
void timecoder_free_lookup(void);

int timecoder_init(struct timecoder_t *tc, const char *def_name);
void timecoder_clear(struct timecoder_t *tc);

void timecoder_monitor_init(struct timecoder_t *tc, int size);
void timecoder_monitor_clear(struct timecoder_t *tc);

int timecoder_submit(struct timecoder_t *tc, signed short *aud,
		     int samples, int rate);

float timecoder_get_pitch(struct timecoder_t *tc);
signed int timecoder_get_position(struct timecoder_t *tc, float *when);
int timecoder_get_alive(struct timecoder_t *tc);
unsigned int timecoder_get_safe(struct timecoder_t *tc);
int timecoder_get_resolution(struct timecoder_t *tc);

#endif
