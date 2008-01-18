*
* $Id: mzchnb.F,v 1.2 2007/05/24 14:41:22 hmd Exp $
*
#include "zebra/pilot.h"
*
      SUBROUTINE MZCHNB (LIX)
C-    Check that LIX is a link in the working space, in a link area,
C-    or even on a local variable, but not a link in a bank.
C-    This is used by routines which restore the input link
C-    from the user back to him, and which could therefor
C-    destroy bank material if a garbage collection happened
C- PN: Can not be used in elastic zebra
      END
