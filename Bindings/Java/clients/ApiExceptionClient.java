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

import java.io.InterruptedIOException;
import java.util.concurrent.TimeoutException;

public class ApiExceptionClient extends PauseClient {
  public ApiExceptionClient (String... arguments) {
    super(arguments);
  }

  @Override
  protected final void runClient (Connection connection)
            throws ProgramException
  {
    ttyMode(
      connection, false,
      (con) -> {
        WriteArguments arguments = new WriteArguments()
          .setRegion(1, (con.getCellCount() + 1))
          .setText("This should fail because the region size is too big.");

        con.write(arguments);
        String label = "API exception";
        String result = null;

        try {
          if (pause(con)) {
            result = String.format("wait for %s timed out", label);
          } else {
            result = String.format("wait for %s interrupted", label);
          }
        } catch (APIException exception) {
          result = String.format("%s received", label);
        }

        printf("%s\n", result);
      }
    );
  }
}
