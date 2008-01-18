      SUBROUTINE  INITHBOOK(TOP,FN,NW,NREC,RECL)
      CHARACTER*(*) FN, TOP
      INTEGER NW,NREC,RECL

      INTEGER ISTAT, LENOCC
#include "zebra/zbcdch.inc"
#include "hbook/hcdire.inc"

*     HBOOL COMMON BLOCK
      CHARACTER*32 RZDIRE
      INTEGER COUNT
      COMMON /GAUDIRZ/ RZDIRE, COUNT

      INTEGER IQUEST
      COMMON /QUEST/ IQUEST(100)

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
C        CALL HLIMIT(250000)
         CALL HLIMIT(NW)
        COUNT = 0
      ELSE
*       Check the same way if HBOOK was already initialized before
*       by somebody else.....
        IF ( HFNAME(1).NE.'COMMON /PAWC/ in memory' ) THEN
C          CALL HLIMIT(-250000)
           CALL HLIMIT(-NW)
          COUNT = 0
        END IF
      ENDIF

      IF (NREC .GT. 0) THEN
         IQUEST(10) = NREC
      END IF

      RECL = MAX(1024,RECL)

      IF ( COUNT.EQ.0 ) THEN
        CALL HROPEN(21,TOP(1:LENOCC(TOP)),FN(1:LENOCC(FN)),
     >              'PN',RECL,ISTAT)
        RZDIRE = '//'//TOP(1:LENOCC(TOP))
        IF ( ISTAT.EQ.0 ) THEN
          CALL HBSET ('BSIZE', 1024, ISTAT)
          IF ( ISTAT.NE.0 ) THEN
        WRITE(6,*)'ERROR IN HBSET!'
          END IF
      ELSE
      WRITE(6,*)'ERROR IN HROPEN!'
        END IF
      END IF
c     COUNT = COUNT + 1
      END

      SUBROUTINE CLOSEHBOOK(TOP,FN)
        CHARACTER*32 RZDIRE
      INTEGER COUNT
        COMMON /GAUDIRZ/ RZDIRE, COUNT
      INTEGER ICYCLE, ISTAT, LENOCC
      CHARACTER*(*) FN, TOP
c     COUNT = COUNT - 1
c     IF ( COUNT.EQ.0 ) THEN
          CALL HREND(TOP(1:LENOCC(TOP)))
      CLOSE(21)
c     END IF
      END
      
      SUBROUTINE F77CLOSE(LUN)
      INTEGER LUN
      CLOSE(LUN)
      END

      INTEGER FUNCTION F77GETLUN()
      IMPLICIT NONE
      INTEGER I
      LOGICAL USABLE
      F77GETLUN = -1
      DO I = 23, 120, 1
        INQUIRE(I, OPENED=USABLE)
        IF ( .NOT.USABLE ) THEN
          F77GETLUN = I
          RETURN
        END IF
      END DO
      END
