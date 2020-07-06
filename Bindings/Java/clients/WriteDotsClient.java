/*
 * libbrlapi - A library providing access to braille terminals for applications.
 *
 * Copyright (C) 2006-2020 by
 *   Samuel Thibault <Samuel.Thibault@ens-lyon.org>
 *   Sébastien Hinderer <Sebastien.Hinderer@ens-lyon.org>
 *
 * libbrlapi comes with ABSOLUTELY NO WARRANTY.
 *
 * This is free software, placed under the terms of the
 * GNU Lesser General Public License, as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option) any
 * later version. Please see the file LICENSE-LGPL for details.
 *
 * Web Page: http://brltty.app/
 *
 * This software is maintained by Dave Mielke <dave@mielke.cc>.
 */

package org.a11y.brlapi.clients;
import org.a11y.brlapi.*;

public class WriteDotsClient extends PauseClient {
  public WriteDotsClient (String... arguments) {
    super(arguments);
    addRepeatingParameter("dots");
  }

  private byte[] cells = null;
  private String text = null;

  @Override
  protected final void processParameters (String[] parameters)
            throws SyntaxException
  {
    int count = parameters.length;
    cells = new byte[count];
    char[] characters = new char[count];

    for (int index=0; index<count; index+=1) {
      byte dots = Parse.asDots("dot numbers", parameters[index]);
      cells[index] = dots;

      char character = 0X2800;
      character |= dots & 0XFF;
      characters[index] = character;
    }

    text = new String(characters);
  }

  @Override
  protected final void runClient (Connection connection) 
            throws ProgramException
  {
    ttyMode(
      connection, false,
      (tty) -> {
        printf("%s\n", text);
        tty.write(cells);
        pause(tty, getPauseTimeout());
      }
    );
  }
}
