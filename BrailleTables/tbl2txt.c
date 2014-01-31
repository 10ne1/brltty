/*
 * BRLTTY - Access software for Unix for a blind person
 *          using a soft Braille terminal
 *
 * Copyright (C) 1995-2000 by The BRLTTY Team, All rights reserved.
 *
 * Web Page: http://www.cam.org/~nico/brltty
 *
 * BRLTTY comes with ABSOLUTELY NO WARRANTY.
 *
 * This is free software, placed under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation.  Please see the file COPYING for details.
 *
 * This software is maintained by Nicolas Pitre <nico@cam.org>.
 */

/* tbl2txt - translate Braille dot table file to a readable form
 * James Bowden
 * $Id: tbl2txt.c,v 1.2 1996/09/21 23:34:51 nn201 Exp $
 * March 1996
 * Version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

unsigned char dot[8] =
{1, 4, 16, 2, 8, 32, 64, 128};	/* Dot values */

int
main (int argc, char *argv[])
{
  FILE *fi, *fo;		/* Input and output file structures */
  int chr;			/* Current character being processed */
  int c;			/* Character read from input */
  int error = 0;		/* error flag */
  int res = 0;			/* Return result, 0=success */
  int i;			/* for loops */

  if (argc != 3)
    {
      fprintf (stderr, "tbl2txt - Braille dot table to text\n");
      fprintf (stderr, "Usage: tbl2txt input_file output_file\n");
      exit (2);
    }

  /* Attempt to open input file */
  if (!(fi = fopen (argv[1], "rb")))
    {
      fprintf (stderr, "tbl2txt: Cannot open input file\n");
      exit (1);
    }

  /* Attempt to open output file */
  if (!(fo = fopen (argv[2], "w")))
    {
      fprintf (stderr, "tbl2txt: Cannot open output file\n");
      fclose (fi);
      exit (1);
    }

  for (chr = 0; chr < 256 && !error; chr++)
    {
      c = fgetc (fi);
      if (c == EOF)
	{
	  error = 1;
	  continue;
	}
      fprintf (fo, " %3d  (", chr);
      for (i = 0; i < 8; i++)
	fputc (((c & dot[i]) ? i + '1' : ' '), fo);
      fputs (")\n", fo);

    }

  if (error == 1)
    {
      fprintf (stderr, "tbl2txt: Table not complete\n");
      res = 1;
    }

  fclose (fi);
  fclose (fo);
  return res;
}
