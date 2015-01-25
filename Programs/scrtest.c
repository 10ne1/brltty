/*
 * BRLTTY - A background process providing access to the console screen (when in
 *          text mode) for a blind person using a refreshable braille display.
 *
 * Copyright (C) 1995-2015 by The BRLTTY Developers.
 *
 * BRLTTY comes with ABSOLUTELY NO WARRANTY.
 *
 * This is free software, placed under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any
 * later version. Please see the file LICENSE-GPL for details.
 *
 * Web Page: http://mielke.cc/brltty/
 *
 * This software is maintained by Dave Mielke <dave@mielke.cc>.
 */

/* scrtest.c - Test program for the screen reading library
 * $Id: scrtest.c,v 1.3 1996/09/24 01:04:27 nn201 Exp $
 */

#include "prologue.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

#include "program.h"
#include "options.h"
#include "log.h"
#include "parse.h"
#include "scr.h"

static char *opt_boxLeft;
static char *opt_boxWidth;
static char *opt_boxTop;
static char *opt_boxHeight;
static char *opt_screenDriver;
static char *opt_driversDirectory;

BEGIN_OPTION_TABLE(programOptions)
  { .letter = 'D',
    .word = "drivers-directory",
    .flags = OPT_Hidden,
    .argument = "directory",
    .setting.string = &opt_driversDirectory,
    .internal.setting = DRIVERS_DIRECTORY,
    .internal.adjust = fixInstallPath,
    .description = "Path to directory for loading drivers."
  },

  { .letter = 'x',
    .word = "screen-driver",
    .argument = "driver",
    .setting.string = &opt_screenDriver,
    .internal.setting = DEFAULT_SCREEN_DRIVER,
    .description = "Screen driver: one of {" SCREEN_DRIVER_CODES "}"
  },

  { .letter = 'l',
    .word = "left",
    .argument = "column",
    .setting.string = &opt_boxLeft,
    .description = "Left edge of region (from 0)."
  },

  { .letter = 'c',
    .word = "columns",
    .argument = "count",
    .setting.string = &opt_boxWidth,
    .description = "Width of region."
  },

  { .letter = 't',
    .word = "top",
    .argument = "row",
    .setting.string = &opt_boxTop,
    .description = "Top edge of region (from 0)."
  },

  { .letter = 'r',
    .word = "rows",
    .argument = "count",
    .setting.string = &opt_boxHeight,
    .description = "Height of region."
  },
END_OPTION_TABLE

static int
setRegion (
  int *offsetValue, const char *offsetOption, const char *offsetName,
  int *sizeValue, const char *sizeOption, int sizeLimit, const char *sizeName
) {
  if (*offsetOption) {
    {
      const int minimum = 0;
      const int maximum = sizeLimit - 1;
      if (!validateInteger(offsetValue, offsetOption, &minimum, &maximum)) {
        logMessage(LOG_ERR, "invalid %s: %s", offsetName, offsetOption);
        return 0;
      }
    }

    if (*sizeOption) {
      const int minimum = 1;
      const int maximum = sizeLimit - *offsetValue;
      if (!validateInteger(sizeValue, sizeOption, &minimum, &maximum)) {
        logMessage(LOG_ERR, "invalid %s: %s", sizeName, sizeOption);
        return 0;
      }
      return 1;
    }
  } else if (*sizeOption) {
    const int minimum = 1;
    const int maximum = sizeLimit;
    if (!validateInteger(sizeValue, sizeOption, &minimum, &maximum)) {
      logMessage(LOG_ERR, "invalid %s: %s", sizeName, sizeOption);
      return 0;
    }
    *offsetValue = (sizeLimit - *sizeValue) / 2;
    return 1;
  } else {
    *offsetValue = sizeLimit / 4;
  }
  if ((*sizeValue = sizeLimit - (*offsetValue * 2)) < 1) *sizeValue = 1;
  return 1;
}

int
main (int argc, char *argv[]) {
  ProgramExitStatus exitStatus;
  void *driverObject;

  {
    static const OptionsDescriptor descriptor = {
      OPTION_TABLE(programOptions),
      .applicationName = "scrtest",
      .argumentsSummary = "[parameter=value ...]"
    };
    PROCESS_OPTIONS(descriptor, argc, argv);
  }

  if ((screen = loadScreenDriver(opt_screenDriver, &driverObject, opt_driversDirectory))) {
    const char *const *parameterNames = getScreenParameters(screen);
    char **parameterSettings;

    if (!parameterNames) {
      static const char *const noNames[] = {NULL};
      parameterNames = noNames;
    }

    {
      const char *const *name = parameterNames;
      unsigned int count;
      char **setting;
      while (*name) ++name;
      count = name - parameterNames;
      if (!(parameterSettings = malloc((count + 1) * sizeof(*parameterSettings)))) {
        logMallocError();
        return PROG_EXIT_FATAL;
      }
      setting = parameterSettings;
      while (count--) *setting++ = "";
      *setting = NULL;
    }

    while (argc) {
      char *assignment = *argv++;
      int ok = 0;
      char *delimiter = strchr(assignment, '=');
      if (!delimiter) {
        logMessage(LOG_ERR, "missing screen parameter value: %s", assignment);
      } else if (delimiter == assignment) {
        logMessage(LOG_ERR, "missing screen parameter name: %s", assignment);
      } else {
        size_t nameLength = delimiter - assignment;
        const char *const *name = parameterNames;
        while (*name) {
          if (strncasecmp(assignment, *name, nameLength) == 0) {
            parameterSettings[name - parameterNames] = delimiter + 1;
            ok = 1;
            break;
          }
          ++name;
        }
        if (!ok) logMessage(LOG_ERR, "invalid screen parameter: %s", assignment);
      }
      if (!ok) return PROG_EXIT_SYNTAX;
      --argc;
    }

    if (constructScreenDriver(parameterSettings)) {
      ScreenDescription description;
      int left, top, width, height;

      describeScreen(&description);
      printf("Screen: %dx%d\n", description.cols, description.rows);
      printf("Cursor: [%d,%d]\n", description.posx, description.posy);

      if (setRegion(&left, opt_boxLeft, "starting column",
                &width, opt_boxWidth, description.cols, "region width")) {
        if (setRegion(&top, opt_boxTop, "starting row",
                  &height, opt_boxHeight, description.rows, "region height")) {
          printf("Region: %dx%d@[%d,%d]\n", width, height, left, top);

          {
            ScreenCharacter buffer[width * height];

            if (readScreen(left, top, width, height, buffer)) {
              int line;
              for (line=0; line<height; line++) {
                int column;
                for (column=0; column<width; column++) {
                  wchar_t character = buffer[line * width + column].text;
                  if (!iswLatin1(character)) {
                    putchar('?');
                  } else if (!isprint(character)) {
                    putchar('*');
                  } else {
                    putchar(character);
                  }
                }
                putchar('\n');
              }
              exitStatus = PROG_EXIT_SUCCESS;
            } else {
              logMessage(LOG_ERR, "Can't read screen.");
              exitStatus = PROG_EXIT_FATAL;
            }
          }
        } else {
          exitStatus = PROG_EXIT_SYNTAX;
        }
      } else {
        exitStatus = PROG_EXIT_SYNTAX;
      }
    } else {
      logMessage(LOG_ERR, "can't open screen.");
      exitStatus = PROG_EXIT_FATAL;
    }

    destructScreenDriver();
  } else {
    logMessage(LOG_ERR, "can't load screen driver.");
    exitStatus = PROG_EXIT_FATAL;
  }
  return exitStatus;
}
