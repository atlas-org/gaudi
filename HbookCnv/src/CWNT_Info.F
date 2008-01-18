      INTEGER FUNCTION CWNTBOOK(ID, CTITLE, COPT)
      IMPLICIT NONE
      INTEGER       ID
      CHARACTER*(*) CTITLE, COPT
      LOGICAL       HEXIST

      IF ( .NOT.HEXIST(ID) ) THEN
        CALL HBNT(ID, CTITLE, COPT) 
        CWNTBOOK = 1
      ELSE
        CWNTBOOK = 0
      END IF
      END

      INTEGER FUNCTION CWNTINFO(NTUPLE_ID,            ! n tuple ID
     >                          VAR_NAME,             ! variable names
     >                          VAR_DIM,              ! # of dimensions of var
     >                          VAR_THEDIM,           ! array with dimensions
     >                          VAR_HASINDEX,         ! flag for index column
     >                          VAR_INDEX,            ! name with index column
     >                          VAR_TYPE,             ! variable type
     >                          VAR_SIZE,             ! variable's packing size
     >                          VAR_HASRANGE,         ! range
     >                          VAR_IRANGE,           ! integer range (if int)
     >                          VAR_FRANGE,           ! floating range (if real)
     >                          NTUPLE_TITLE,         ! N tuple title
     >                          NTUPLE_DIM,           ! # of variables in tuple
     >                          NTUPLE_NENT,          ! # of entries in tuple
     >                          NTUPLE_BLOCK)         ! all ntuple block names
*.==========>
*.
*.           To print statistics about a new N-tuple
*.
*..=========> ( A.A.Rademakers )
*  Hacked up to retrieve information in one structure by M.Frank
*
      CHARACTER*(*) VAR_NAME(*)
      INTEGER       VAR_DIM(*)
      INTEGER       VAR_THEDIM(5,*)
      INTEGER       VAR_HASINDEX(*)
      CHARACTER*(*) VAR_INDEX(*)
      CHARACTER*1   VAR_TYPE(*)
      INTEGER       VAR_SIZE(*)
      INTEGER       VAR_HASRANGE(*)
      INTEGER       VAR_IRANGE(2,*)
      REAL*8        VAR_FRANGE(2,*)
      INTEGER       NTUPLE_ID
      CHARACTER*(*) NTUPLE_TITLE
      INTEGER       NTUPLE_NENT
      INTEGER       NTUPLE_DIM
      CHARACTER*(*) NTUPLE_BLOCK(*)
#include "hbook/hcntpar.inc"
#include "hbook/hcnt.inc"
#include "hbook/hcflag.inc"
#include "hbook/hcbook.inc"
#include "hbook/hcbits.inc"
#include "hbook/hcunit.inc"
*
      CHARACTER*8 NTUPLE_BLK
      CHARACTER*32 NAME, SUBS
      LOGICAL      VTUP, LDUM
      INTEGER      IENT
*
      IENT = 0

      ID    = NTUPLE_ID
      IDPOS = LOCATI(IQ(LTAB+1),IQ(LCDIR+KNRH),ID)
      IF (IDPOS .LE. 0) THEN
         CWNTINFO = 0
         RETURN
      ENDIF
      LCID  = LQ(LTAB-IDPOS)
      I4 = JBIT(IQ(LCID+KBITS),4)
      IF (I4 .EQ. 0) RETURN
      IF (IQ(LCID-2) .NE. ZLINK) THEN
         CWNTINFO = 0
         RETURN
      ENDIF
*
      ICOLS = 0
      VTUP  = .FALSE.
*
      LBLOK = LQ(LCID-1)
      LCHAR = LQ(LCID-2)
      LINT  = LQ(LCID-3)
      LREAL = LQ(LCID-4)
*
      ITIT1 = IQ(LCID+ZITIT1)
      NWTIT = IQ(LCID+ZNWTIT)
*
*-- loop over all blocks
*
  5   LNAME = LQ(LBLOK-1)
*
      IOFF = 0
      NTUPLE_TITLE = ' '
      NTUPLE_BLK = ' '
      CALL UHTOC(IQ(LBLOK+ZIBLOK), 4, NTUPLE_BLK, 8)
      CALL UHTOC(IQ(LCID+ITIT1),   4, NTUPLE_TITLE, NWTIT*4)
      NTUPLE_DIM   = IQ(LBLOK+ZNDIM)
      NTUPLE_NENT  = IQ(LCID+ZNOENT)
*
      DO 10 I = 1, NTUPLE_DIM
         IENT = IENT + 1
         NTUPLE_BLOCK(IENT) = NTUPLE_BLK
*         WRITE(*,*) 'NTUPLE_BLOCK: ',NTUPLE_BLK, NTUPLE_BLOCK(IENT)

         CALL HNDESC(IOFF, NSUB, ITYPE, ISIZE, NBITS, LDUM)
         LL = IQ(LNAME+IOFF+ZLNAME)
         LV = IQ(LNAME+IOFF+ZNAME)
         CALL UHTOC(IQ(LCHAR+LV), 4, NAME, LL)
         VAR_NAME(IENT)(1:LL) = NAME(1:LL)


         VAR_DIM(IENT)  = 0
         VAR_SIZE(IENT)  = NBITS
         IELEM = 1
         IF (NSUB .GT. 0) THEN
            DO 20 J = 1, NSUB
               LP = IQ(LINT+IQ(LNAME+IOFF+ZARIND)+(J-1))
               IF (LP .LT. 0) THEN
                  IE = -LP
                  VAR_DIM(IENT) = VAR_DIM(IENT) + 1
              VAR_THEDIM(VAR_DIM(IENT),IENT) = IE
              VAR_HASINDEX(IENT) = 0
               ELSE
                  LL = IQ(LNAME+LP-1+ZLNAME)
                  LV = IQ(LNAME+LP-1+ZNAME)
                  CALL UHTOC(IQ(LCHAR+LV), 4, SUBS, LL)
                  LL1 = IQ(LNAME+LP-1+ZRANGE)
                  IE  = IQ(LINT+LL1+1)
                  VTUP = .TRUE.
              VAR_HASINDEX(IENT) = 1
              VAR_INDEX(IENT) = SUBS(1:LENOCC(SUBS))
               ENDIF
               IELEM = IELEM*IE
*
  20        CONTINUE
            IF ( VAR_HASINDEX(IENT).EQ.1 ) THEN
              VAR_DIM(IENT) = VAR_DIM(IENT) + 1
            END IF
         ENDIF
*
         IF (IQ(LNAME+IOFF+ZRANGE) .NE. 0) THEN
            LP = IQ(LNAME+IOFF+ZRANGE)
            IF (ITYPE .EQ. 1) THEN
               VAR_FRANGE(1,IENT) = Q(LREAL+LP)
               VAR_FRANGE(2,IENT) = Q(LREAL+LP+1)
            ELSEIF (ITYPE.EQ.2 .OR. ITYPE.EQ.3) THEN
               VAR_IRANGE(1,IENT) = IQ(LINT+LP)
               VAR_IRANGE(2,IENT) = IQ(LINT+LP+1)
            ELSEIF (ITYPE.EQ.4) THEN
               VAR_IRANGE(1,IENT) = 0
               VAR_IRANGE(2,IENT) = 1
            ENDIF
            VAR_HASRANGE(IENT) = 1
         ENDIF
*
         IF (ITYPE .EQ. 1) THEN
            VAR_TYPE(IENT) = 'R'
         ELSEIF (ITYPE .EQ. 2) THEN
            VAR_TYPE(IENT) = 'I'
         ELSEIF (ITYPE .EQ. 3) THEN
            VAR_TYPE(IENT) = 'U'
         ELSEIF (ITYPE .EQ. 4) THEN
            VAR_TYPE(IENT) = 'L'
         ELSEIF (ITYPE .EQ. 5) THEN
            VAR_TYPE(IENT) = 'C'
         ENDIF
*
         ICOLS = ICOLS + IELEM
         IOFF  = IOFF + ZNADDR
  10  CONTINUE
*
      LBLOK = LQ(LBLOK)
      IF (LBLOK .NE. 0) GOTO 5
*
      CWNTINFO = 1

*      DO 100 II = 1 , IENT
*         WRITE(*,*) NTUPLE_BLOCK(II), VAR_NAME(II)
* 100  CONTINUE

      NTUPLE_DIM = IENT

      END
