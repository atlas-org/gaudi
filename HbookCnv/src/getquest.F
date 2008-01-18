C $Id: getquest.F,v 1.1 2005/12/02 18:04:36 hmd Exp $
      INTEGER FUNCTION GETQUEST( INDEX ) 
C-----------------------------------------------------------------------
C  Purpose   : 
C  Arguments : 
C
C  Author    : Ivan BELYAEV Ivan.Belyaev@itep.ru
C  Created   : 2005-11-25
C-----------------------------------------------------------------------
      IMPLICIT NONE
C- Include Files
      INTEGER IQUEST,INDEX
#include "zebra/quest.inc"
C- Local Variables
      
      GETQUEST = IQUEST( INDEX ) 
      
      RETURN
      END
      
