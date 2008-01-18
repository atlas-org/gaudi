      INTEGER FUNCTION NTUPLETYPE(ID1)
*.==========>
*.
*.           To print statistics about a new N-tuple
*.
*..=========> ( A.A.Rademakers )
*
#include "hbook/hcntpar.inc"
#include "hbook/hcnt.inc"
#include "hbook/hcflag.inc"
#include "hbook/hcbook.inc"
#include "hbook/hcbits.inc"
#include "hbook/hcunit.inc"
      INTEGER ID1, IRET

      NTUPLETYPE = 0
      ID         = ID1
      IDPOS      = LOCATI(IQ(LTAB+1),IQ(LCDIR+KNRH),ID)
      IF (IDPOS .LE. 0) THEN
         NTUPLETYPE = 0
         RETURN
      ENDIF
      LCID  = LQ(LTAB-IDPOS)
      I4 = JBIT(IQ(LCID+KBITS),4)
      IF (I4 .EQ. 0)  THEN
         NTUPLETYPE = 0
         RETURN
      END IF
*     Old N-tuples
      IF (IQ(LCID-2) .NE. ZLINK) THEN
         NTUPLETYPE = 2
         RETURN
      ENDIF
*     New N-tuples
      NTUPLETYPE = 1
      END




