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

/*
 * brl_dl.c - handling of dynamic drivers
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "brl.h"
#include "scr.h"
#include "misc.h"

driver *thedriver = NULL;	/* filled by dynamic libs */
char* driver_libname = NULL;	/* name of library */

static void* library = NULL;	/* handle to driver */

/* remove driver - called via atexit */
/* is this needed ? */
void driver_unload()
{
  /* printf("unloading driver\n"); */
  dlclose(library);
}

/* load driver from libray */
/* return 0 on success */
int driver_load()
{
  const char* error;
  static char tmp[] = "libbrltty??.so.1"; /* two ? for shortcut */

  /* allow shortcuts */
  if (driver_libname[2] == 0)
    {
      char * pos = strchr(tmp, '?');
      pos[0] = driver_libname[0];
      pos[1] = driver_libname[1];
      driver_libname = tmp;
    }

  /* set the search path for libraries - don't overwite*/
  setenv("LD_LIBRARY_PATH", LIB_PATH, 0);

  library = dlopen(driver_libname, RTLD_NOW);
  if (library == NULL) 
    {
      LogAndStderr(LOG_ERR, "could not open library %s: %s",
		   driver_libname, dlerror()); 
      return 1;
    }
  atexit(driver_unload);	/* unload befor exit - no error checking */
  dlerror();			/* clear any errors */

  thedriver = dlsym(library, "brl_driver"); /* locate struct driver - filled with all the data */
  error = dlerror();
  if (error)
    {
      LogAndStderr(LOG_ERR, "could not find 'brl_driver': %s", error);
      return 2;
    }
  return 0;
}



