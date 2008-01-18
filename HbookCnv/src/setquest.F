C $Id: setquest.F,v 1.1 2005/12/02 18:04:36 hmd Exp $
      INTEGER FUNCTION SETQUEST( INDEX , IVALUE ) 
C-----------------------------------------------------------------------
C  Purpose   : 
C  Arguments : 
C
C  Author    : Ivan BELYAEV
C  Created   : 2005-11-25
C-----------------------------------------------------------------------
      IMPLICIT NONE
C- Include Files
      INTEGER IQUEST,INDEX,IVALUE
#include "zebra/quest.inc"
      INTEGER  GETQUEST
      EXTERNAL GETQUEST 
C- Local Variables
C-----------------------------------------------------------------------
      IQUEST( INDEX ) = IVALUE 
      SETQUEST = GETQUEST( INDEX ) 
  999 RETURN
      END
