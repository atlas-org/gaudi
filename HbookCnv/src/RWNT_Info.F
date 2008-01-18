      INTEGER FUNCTION RWNTBOOK(ID, DIM, VNAME, CDIR, TITLE)
      IMPLICIT NONE
        INTEGER       ID, DIM, IVNAME
        CHARACTER*(*) CDIR, TITLE
        CHARACTER*(*) VNAME(*)
*        RECORD / NTUPLEINFO / INFO
        INTEGER       LENOCC, I
        LOGICAL       EXISTS, HEXIST
        EXTERNAL      LENOCC, HEXIST
        EXISTS = HEXIST(ID)
      IF ( .NOT.EXISTS ) THEN
          CALL HBOOKN(ID,
     >                TITLE(1:LENOCC(TITLE)),
     >                DIM,
     >                CDIR,
     >                1024,
     >                VNAME)
          RWNTBOOK = 1
      ELSE
      RWNTBOOK = 0
        END IF
      END

      INTEGER FUNCTION RWNTINFO(ID1, DIM, NENT, VNAME, RANGE, TITLE)
*.==========>
*.           To print statistics about a N-tuple
*.
*..=========> ( R.Brun )
*  Hacked up to retrieve information in one structure by M.Frank
*
#include "hbook/hcflag.inc"
#include "hbook/hcbook.inc"
#include "hbook/hcbits.inc"
#include "hbook/hcunit.inc"
      INTEGER ID1, DIM, NENT
      REAL*8 RANGE(2, *)
      CHARACTER*(*) TITLE, VNAME(*)

      CHARACTER*8 VVNAME
      INTEGER BUFFER(2)
      DIMENSION NAME(2)
      EQUIVALENCE (BUFFER(1), VVNAME)
*.___________________________________________
*
      RWNTINFO = 0
      ID = ID1
      IDPOS=LOCATI(IQ(LTAB+1),IQ(LCDIR+KNRH),ID)

      IF ( IDPOS.LE.0 ) THEN
        RETURN
      END IF
      LCID=LQ(LTAB-IDPOS)
      LCONT=LQ(LCID-3)
      I4=JBIT(IQ(LCID+KBITS),4)
      IF ( I4.EQ.0 .OR. IQ(LCID-2) .NE. 2) THEN
        RETURN
      END IF
*
      LLIMS       = LQ(LCID-2)
      ITAG1=IQ(LCID+10)
      ITIT1=IQ(LCID+9)
      NWTIT=IQ(LCID+8)
*
      DIM   = IQ(LCID+2)
      NENT  = IQ(LCID+3)
      CALL UHTOC(IQ(LCID+ITIT1), 4, TITLE, NWTIT*4)
      DO 10 I = 1, DIM
        CALL UCOPY(IQ(LCID+ITAG1+2*(I-1)),BUFFER(1),2)
        VNAME(I)(1:LENOCC(VVNAME)) = VVNAME(1:LENOCC(VVNAME))
        RANGE(1,I) = Q(LLIMS+2*I-1)
        RANGE(2,I) = Q(LLIMS+2*I)
  10  CONTINUE
      RWNTINFO = 1
*
      END
