// omap3530-pwm -- Control PWM outputs on the TI BeagleBoard C3 or C4
// Copyright (c) 2010 Thomas W. Most <tomwm@freecog.net>
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation; either version 2.1 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

// 13 MHz clock
#define PWM_FREQUENCY_13MHZ 13000000
// 32 kHz clock
#define PWM_FREQUENCY_32KHZ 32000

guint8 *pwm_mmap_instance(int mem_fd, int instance_number);
int pwm_munmap_instance(guint8 *instance);
int pwm_config_clock(int mem_fd, gboolean gptimer10_13mhz, gboolean gptimer11_13mhz);
guint32 pwm_calc_resolution(int pwm_frequency, int clock_frequency);
void pwm_config_timer(guint8 *instance, guint32 resolution, float duty_cycle);
int pwm_open_devmem(void);
void pwm_close_devmem(int dev_fd);
