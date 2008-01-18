// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/HbookCnv/src/HbookDef.h,v 1.10 2005/12/02 14:45:52 hmd Exp $
#ifndef HBOOKCNVSVC_HBOOKDEF_H
#define HBOOKCNVSVC_HBOOKDEF_H 1

// Include files
#include <string>

//------------------------------------------------------------------------------
//
// Description: Definitions of HBOOK subroutines used in HbookCnv
//
// Author:      Pavel Binko
//
//------------------------------------------------------------------------------

class NTUPLEINFO;

// Global flag to switch between explicit data types 
// for row-wise N-tuples and FLOAT only version
enum DataTypePolicy { USE_DATA_TYPES, FLOAT_ONLY, FORTRAN_STYLE, HUNGARIAN_STYLE };
extern long dataTypePolicy();

inline void cleanForString(char* title, long len)   {
  for ( int i = len; i >= 0; i-- )    {
    if ( title[i]==' ' || title[i] == 0 ) title[i] = 0;
    else if ( title[i]!=' ' ) break;
  }
  title[len] = 0;
}

extern "C" {

// This structure MUST map to the upper fortran structure
#ifdef _WIN32
#define HBOOK_CALL_TYPE __stdcall
  void HBOOK_CALL_TYPE HBOOK1( const long&, const char*, int, const long&, const float&, 
                const float&, const float& );
  void HBOOK_CALL_TYPE HBOOKB( const long&, const char*, int, const long&, const float*, const float& );
  void HBOOK_CALL_TYPE HBOOK2( const long&, const char*, int, const long&, const float&, 
                const float&, const long&, const float&,
                const float&, const float& );
  void HBOOK_CALL_TYPE CLOSEHBOOK(const char*, int, const char*, int);
  void HBOOK_CALL_TYPE INITHBOOK(const char*, int, const char*, int,
				 const int&, const int&, const int&);
  void HBOOK_CALL_TYPE INITHBOOKNF(const int&);
  void HBOOK_CALL_TYPE SETMEANRMS( const long&, const float&, const float&, const float&, const float& );
  void HIDOPT(const long&, const char*, int);
  void HBOOK_CALL_TYPE HGIVE (const int&, char*, int, int&, float&, float&, int&, float&, float&, int&, int&);
  void HBOOK_CALL_TYPE HUNPAK(const int&, float*, const char*, int, int&);
  void HBOOK_CALL_TYPE HUNPKE(const int&, float*, const char*, int, int&);
  float HBOOK_CALL_TYPE HSTATI(const int&, const int&, const char*, int, const int&);

  int  HBOOK_CALL_TYPE  GETQUEST( const int&              ) ;
  int  HBOOK_CALL_TYPE  SETQUEST( const int& , const int& ) ;
  
#else
#define HBOOK_CALL_TYPE
  void hbook1_( const long&, const char*, const long&, const float&, 
                const float&, const float&, int );
  void hbookb_( const long&, const char*, const long&, const float*, const float&, int );
  void hbook2_( const long&, const char*, const long&, const float&, 
                const float&, const long&, const float&, 
                const float&, const float&, int );
  void closehbook_(const char*, const char*, int, int);
  void inithbook_(const char*, const char*, const int&, const int &,
		  const int&, int, int);
  void inithbooknf_(const int&);
  void setmeanrms_(const long&, const float&, const float&, const float&, const float&);
  void hidopt_(const int&, const char*, int);
  void hgive_ (const int&, char*, int&, float&, float&, 
                                  int&, float&, float&, 
                                  int&, int&, int);
  void hunpak_(const int&, float*, const char*, const int&, int);
  void hunpke_(const int&, float*, const char*, const int&, int);
  float hstati_(const int&, const int&, const char*, const int&, int);

  int   getquest_( const int& ) ;
  int   setquest_( const int& , const int& ) ;
  
#define HROUT       hrout_
#define HDELET      hdelet_
#define HFILL       hfill_
#define HPAK        hpak_
#define HPAKE       hpake_
#define HFILL       hfill_
#define HPRINT      hprint_
#define HEXIST      hexist_
#define HRESET      hreset_
#define HPRNT       hprnt_
#define HFNTB       hfntb_
#define HFNT        hfnt_
#define HFN         hfn_
#define HGNT        hgnt_
#define HGN         hgn_
#define HGNF        hgnf_
#define HGNPAR      hgnpar_
#define HRIN        hrin_
#define NTUPLETYPE  ntupletype_
#define HREND       hrend_
#define F77CLOSE    f77close_
#define F77GETLUN   f77getlun_
#define HNOENT      hnoent_
#define HXYIJ       hxyij_
#define HIJXY       hijxy_
#define HX          hx_
#define HXE         hxe_
#define HI          hi_
#define HIX         hix_
#define HIE         hie_
#define HIJ         hij_
#define HIJE        hije_
#define HXY         hxy_
#define HKIND       hkind_

#define GETQUEST   getquest_ 
#define SETQUEST   setquest_ 

#endif
  void HBOOK_CALL_TYPE HROUT(const long&, int*, const char*, int);
  void HBOOK_CALL_TYPE HDELET(const long&);
  void HBOOK_CALL_TYPE HFILL  (const long&, const float&, const float&, const float& );
  void HBOOK_CALL_TYPE HPAK   (const long&, const void*);
  void HBOOK_CALL_TYPE HPAKE  (const long&, const void*);

  void HBOOK_CALL_TYPE F77CLOSE(const long&);
  long HBOOK_CALL_TYPE F77GETLUN();
  void HBOOK_CALL_TYPE HPRINT (const long& id);
  long HBOOK_CALL_TYPE HEXIST (const long& id);
  void HBOOK_CALL_TYPE HPRNT  (const long& id);
  void HBOOK_CALL_TYPE HFNTB  (const long& id, const char* title, int);
  void HBOOK_CALL_TYPE HFNT   (const long& id);
  void HBOOK_CALL_TYPE HFN    (const long& id, const float* buffer);
  void HBOOK_CALL_TYPE HNOENT (const long& id, long& nument);
  void HBOOK_CALL_TYPE HRESET (const long& id, const char* title, int);
  void HBOOK_CALL_TYPE HRIN   (const long&, const long&, const long&);
  void HBOOK_CALL_TYPE HGNT   (const long&, const long&, long&);
  void HBOOK_CALL_TYPE HPRNTU (const long&);
  void HBOOK_CALL_TYPE HGNPAR (const long&, const char*, long);
  void HBOOK_CALL_TYPE HGNF   (const long&, const long&, float*, long&);
  void HBOOK_CALL_TYPE HGN    (const long&, long&, const long&, float*, long&);
  void HBOOK_CALL_TYPE HREND  (const char*, int);
  void HBOOK_CALL_TYPE HKIND  (const int&, long*, const char*, int);
  // Calls with interface...
  long HBOOK_CALL_TYPE NTUPLETYPE  (const long&);
  void HBOOK_CALL_TYPE HIX    (const long&, const long&, float&);
  void HBOOK_CALL_TYPE HXYIJ  (const long&, const float&, const float&, long&, long&);
  void HBOOK_CALL_TYPE HIJXY  (const long&, const long&, const long&, float&, float&);
  float HBOOK_CALL_TYPE HX    (const long&, const float&);
  float HBOOK_CALL_TYPE HXE   (const long&, const float&);
  float HBOOK_CALL_TYPE HXY   (const long&, const float&, const float&);
  float HBOOK_CALL_TYPE HXYE  (const long&, const float&, const float&);
  float HBOOK_CALL_TYPE HI    (const long&, const long&);
  float HBOOK_CALL_TYPE HIE   (const long&, const long&);
  float HBOOK_CALL_TYPE HIJ   (const long&, const long&, const long&);
  float HBOOK_CALL_TYPE HIJE  (const long&, const long&, const long&);
}

  void HLDIR(const std::string& a, const std::string& b = " ");
  void HCDIR(const std::string& a, const std::string& b = " ");
  void HMDIR(const std::string& a, const std::string& b = " ");
  void HBNAME (const long& id, const std::string& a, const void* opt, const std::string& b);
  void HROPEN(const long& lun, const std::string& a, const std::string& b, const std::string& c, long& i, long& j);
  void HLNEXT(long& id, std::string& a, std::string& b, const std::string& c);
  long RWNT_BOOK(const NTUPLEINFO& );
  long RWNT_INFO(NTUPLEINFO& ); 
  long CWNT_BOOK(const NTUPLEINFO& );
  long CWNT_INFO(NTUPLEINFO& ); 

#ifdef _WIN32
inline void HBOOK1(int id, const std::string& t, int nx, float l, float h)   
{  HBOOK1(id, t.data(), t.length(), nx, l, h, 0.0);                           }
inline void HBOOKB(int id, const std::string& t, int nx, float* l) 
{  HBOOKB(id, t.data(), t.length(), nx, l, 0.0);                              }
inline void HBOOK2(int id, const std::string& t, int nx, float l, float h, int ny, float x, float y)   
{  HBOOK2(id, t.data(), t.length(), nx, l, h, ny, x, y, 0.0);                 }
inline void CLOSE_HBOOK(const std::string& a, const std::string& b)  
{  CLOSEHBOOK(a.data(), a.length(), b.data(), b.length());                    }
inline void INIT_HBOOK(const std::string& a, const std::string& b,
		       const int nw, const int nrec, const int recl)  
{  INITHBOOK(a.data(), a.length(), b.data(), b.length(), nw, nrec, recl);     }
inline void INIT_HBOOK(const int nw)  
{  INITHBOOKNF(nw);                                                           }
inline void SET_MEAN_RMS(long histoID, float mean, float rms, float sumOfHeights, float equivEntries)   
{  SETMEANRMS(histoID, mean, rms, sumOfHeights, equivEntries);                }
inline void HIDOPT(int id, const std::string& t)   
{  HIDOPT(id, t.data(), t.length());                                          }
inline void HUNPAK(int id, float* buff, const std::string& opt, int nslice = 0)  
{  HUNPAK(id, buff, opt.data(), opt.length(), nslice);                        }
inline void HUNPKE(int id, float* buff, const std::string& opt, int nslice = 0)
{  HUNPKE(id, buff, opt.data(), opt.length(), nslice);                        }
inline float HSTATI(int id, int what, const std::string& opt, int nslice = 0)
{  return HSTATI(id, what, opt.data(), opt.length(), nslice);                 }
inline void HGIVE (int id, std::string& title, 
                   int& nx, float& xmi, float& xma, 
                   int& ny, float& ymi, float& yma, 
                   int& nwd, int& loc)    {
  char tit[80];
  HGIVE(id, tit, sizeof(tit), nx, xmi, xma, ny, ymi, yma, nwd, loc);
  cleanForString(tit, sizeof(tit)-1);
  title = tit;
}

#else

inline void HBOOK1(int id, const std::string& t, int nx, float l, float h)   
{  hbook1_(id, t.data(), nx, l, h, 0.0, t.length());                          }
inline void HBOOKB(int id, const std::string& t, int nx, float* l) 
{  hbookb_(id, t.data(), nx, l, 0.0, t.length());                             }
inline void HBOOK2(int id, const std::string& t, int nx, float l, float h, int ny, float x, float y)   
{  hbook2_(id, t.data(), nx, l, h, ny, x, y, 0.0, t.length());                }
inline void CLOSE_HBOOK(const std::string& a, const std::string& b)  
{  closehbook_(a.data(), b.data(), a.length(), b.length());                   }
inline void INIT_HBOOK(const std::string& a, const std::string& b,
		       const int nw, const int nrec, const int recl)  
{  inithbook_(a.data(), b.data(), nw, nrec, recl, a.length(), b.length());    }
inline void INIT_HBOOK(const int nw)  
{  inithbooknf_(nw);                                                          }
inline void SET_MEAN_RMS(long histoID, float mean, float rms, float sumOfHeights, float equivEntries)   
{  setmeanrms_(histoID, mean, rms, sumOfHeights, equivEntries);               }
inline void HIDOPT(int id, const std::string& t)   
{  hidopt_(id, t.data(), t.length());                                         }
inline void HUNPAK(int id, float* buff, const std::string& opt, int nslice = 0)  
{  hunpak_(id, buff, opt.data(), nslice, opt.length());                       }
inline void HUNPKE(int id, float* buff, const std::string& opt, int nslice = 0)
{  hunpke_(id, buff, opt.data(), nslice, opt.length());                       }
inline void HGIVE (int id, std::string& title, 
                   int& nx, float& xmi, float& xma, 
                   int& ny, float& ymi, float& yma, 
                   int& nwd, int& loc)    {
  char tit[80];
  hgive_(id, tit, nx, xmi, xma, ny, ymi, yma, nwd, loc, sizeof(tit));
  cleanForString(tit, sizeof(tit)-1);
  title = tit;
}
inline float HSTATI(int id, int what, const std::string& opt, int nslice = 0)
{  return hstati_(id, what, opt.data(), nslice, opt.length());                 }

#endif

#endif   // HBOOKCNVSVC_HBOOKDEF_H



