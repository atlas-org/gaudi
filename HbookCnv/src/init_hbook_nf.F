C Do just the HLIMIT call

      SUBROUTINE  INITHBOOKNF(NW)
      INTEGER NW,IQ
      INTEGER ISTAT, LENOCC      
#include "zebra/zbcdch.inc"
#include "hbook/hcdire.inc"

*     HBOOL COMMON BLOCK
      CHARACTER*32 RZDIRE
      INTEGER COUNT
      COMMON /GAUDIRZ/ RZDIRE, COUNT

C      INTEGER HMEMOR(250000)
C      COMMON/PAWC/HMEMOR

      CHARACTER*255 TESTPATTERN
      DATA TESTPATTERN /
     >'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-*/()$= ,.#''!:"_]&@?[><\^;%
     >abcdefghijklmnopqrstuvwxyz{|}~`?'/
*     I know it's a bad hack, but I do not see any other way than
*     checking against a variable in the static ZEBRA area....
*     M.Frank
      
      NW = ABS(NW)

      ISTAT = 0
      IF ( CQALLC(1:20).NE.TESTPATTERN(1:20) ) THEN
        CALL HLIMIT(NW)
        COUNT = 0
      ELSE
*       Check the same way if HBOOK was already initialized before
*       by somebody else.....
        IF ( HFNAME(1).NE.'COMMON /PAWC/ in memory' ) THEN
          CALL HLIMIT(-NW)
          COUNT = 0
        END IF
      ENDIF

      END

