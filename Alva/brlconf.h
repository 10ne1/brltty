/*
 * BRLTTY - Access software for Unix for a blind person
 *          using a soft Braille terminal
 *
 * Copyright (C) 1995-1999 by The BRLTTY Team, All rights reserved.
 *
 * Nicolas Pitre <nico@cam.org>
 * St�phane Doyon <s.doyon@videotron.ca>
 * Nikhil Nair <nn201@cus.cam.ac.uk>
 *
 * BRLTTY comes with ABSOLUTELY NO WARRANTY.
 *
 * This is free software, placed under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation.  Please see the file COPYING for details.
 *
 * This software is maintained by Nicolas Pitre <nico@cam.org>.
 */

/* Alva/brlconf.h - Configurable definitions for the Alva driver
 * Copyright (C) 1995-1998 by Nicolas Pitre <nico@cam.org>
 *
 * Edit as necessary for your system.
 */



/* Known Device Identification Numbers (not to be changed) */
#define ABT_AUTO	-1	/* for new firmware only */
#define ABT320          0
#define ABT340          1
#define ABT34D          2
#define ABT380          3	/* ABT340 Desktop */
#define ABT38D          4	/* ABT380 Twin Space */
#define DEL440		11	/* Alva Delphi 40 */
#define DEL480		13 	/* Delphi Multimedia */


/***** User Settings *****/


/* Define next line to 1 if you have a firmware older than version 010495 */
#define ABT3_OLD_FIRMWARE 0


/* Specify the terminal model that you'll be using.
 * Use one of the previously defined device ID.
 * WARNING: Do not use BRL_AUTO if you have ABT3_OLD_FIRMWARE defined to 1 !
 */
#define MODEL   ABT_AUTO


/* Define next line to 1 to use parallel port interface.
 * Nothe that this requires the appropriate extra library and modification
 * in the Makefile.  See the README file for details.
 */
#define USE_PARALLEL_PORT 0


/* If you defined USE_PARALLEL_PORT to 1, this is where you should define
 * the address of the parallel port your ABT will be connected to.
 */
#define LPT_PORT 0x378


/* If you defined USE_PARALLEL_PORT to 0, then the serial line baudrate
 * is defined here.  Note that default braille serial device is defined 
 * in ../Makefile
 */
#define BAUDRATE B9600


/* typematic settings */
#define TYPEMATIC_DELAY 10	/* nbr of cycles before a key is repeated */
#define TYPEMATIC_REPEAT 2	/* nbr of cycles between each key repeat */


/* Delay in miliseconds between forced full refresh of the display.
 * This is to minimize garbage effects due to noise on the serial line.
 */
#define REFRESH_RATE 1000

