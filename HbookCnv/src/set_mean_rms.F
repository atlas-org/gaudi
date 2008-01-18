      SUBROUTINE  SETMEANRMS(HISTOID,MEAN,RMS,SUMHEIGHTS,EQUIVENTRIES)
*            
#include "hbook/hcbook.inc"
      INTEGER HISTOID      
      REAL MEAN,RMS,SUMHEIGHTS,EQUIVENTRIES
      DOUBLE PRECISION SWX(2)

*
* Set local pointers for histogram in hbook/zebra common block
      CALL HFIND(HISTOID,'SETMEANRMS')
*
* Fill corresponding values in hbook/zebra common block 
      Q(LCONT+KSTAT1) = SUMHEIGHTS
	Q(LCONT+KSTAT1+1) = ( SUMHEIGHTS*SUMHEIGHTS ) / EQUIVENTRIES
	SWX(1) = MEAN * SUMHEIGHTS
	SWX(2) = ( RMS*RMS + MEAN*MEAN ) * SUMHEIGHTS
      CALL UCOPY(SWX,Q(LCONT+KSTAT1+2),4)
*
      RETURN
      END
