* $Id: mzpaw.F,v 1.4 2007/05/24 14:41:22 hmd Exp $
*
      SUBROUTINE MZPAW (NWORDS,CHOPT)
C-    Initialize the store /PAWC/ of at least 250000 words;
C--------------------------------------------------------------------
*CDE,PAWC. - some variables are renamed !  (wws->ipaw etc)
      PARAMETER (IALLOC = 250000)
      COMMON /PAWC/  NWPAW,IXPAWC,IHBOOK,IXHIGZ,IXKU,IFENCE(5),
     +               LMAIN, IPAW(IALLOC+1000)
      CHARACTER*(*) CHOPT

      NW   = abs (NWORDS)
      NW   = max (NW,100000)
      I1   = LOCF(IPAW)
      IF ( NW .LE. IALLOC ) THEN
        I2   = LOCF(IPAW)
      ELSE
C       WRITE(*,*) 'Resizing /PAWC/ to ',NW,' words'
        I2   = MEMGETF (NW+1000)
        IF (I2 .EQ. 0) THEN
          PRINT *,' MZPAW: requested memory not found '
          I2 = LOCF(IPAW)
          NW = IALLOC
        ELSE IF (I2.LT.I1) THEN
          PRINT *,' MZPAW: allocated memory in front of existing /PAWC/'
          PRINT *,'        extension not possible.'
          I2 = LOCF(IPAW)
          NW = IALLOC
        ENDIF
      ENDIF
      LL   = max(I2-I1,0)
      NW   = NW + LL
      KWW  = LL + 100
C
      CALL MZSTOR (IXPAWC,'/PAWC/',' ',IFENCE,LMAIN,IPAW,IPAW,
     +                                 IPAW(KWW+100),IPAW(NW-100))
      CALL MZWORK (IXPAWC,IPAW(9989),IPAW(KWW),5)
      CALL MZWORK (IXPAWC,IPAW(50),IPAW(5000),0)
c
      NWPAW  = NW
      IHBOOK = 0
      IXHIGZ = 0
      IXKU   = 0
      RETURN
      END


