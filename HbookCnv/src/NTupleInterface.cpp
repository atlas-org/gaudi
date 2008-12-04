#include "GaudiKernel/NTuple.h"
#include "HbookDef.h"
#include "NTupleInfo.h"
#include <climits>

#ifdef _WIN32
extern"C" {
 long HBOOK_CALL_TYPE RWNTBOOK( const long& id, 
                                const long& dim, 
                                const char tag[512][64], long,
                                const char* rzdir, long, 
                                const char* title, long);
 long HBOOK_CALL_TYPE RWNTINFO( const long& id, 
                                long& dim, 
                                long& ents, 
                                char tags[512][64], long,
                                double range[512][2], 
                                char title[], long);
 long HBOOK_CALL_TYPE CWNTBOOK( const long& id, 
                                const char title[], long,
                                const char opt[], long);
 long HBOOK_CALL_TYPE CWNTINFO( const long& id, 
                                char name[512][64], long,
                                long dim[], 
                                long theDim[512][5],
                                long hasIndex[], 
                                char index[512][64], long,
                                char type[], long, 
                                long size[], 
                                long hasRange[],
                                long irange[512][2], 
                                double frange[512][2], 
                                char title[], long, 
                                long& numVar, 
                                long& numEnt, 
				char block[512][12], long);
  void HBOOK_CALL_TYPE HBNAME  (const long& id, 
        const char* title, int, 
        const void* opt, 
        const char* form, int);

  void HBOOK_CALL_TYPE HROPEN  (const long& lun, 
        const char*, int, 
        const char*, int, 
        const char*, int, 
        long&, long&);
  void HBOOK_CALL_TYPE HLNEXT  (long& id, 
        char*, int, 
        char*, int, 
        const char*, int);
  void HBOOK_CALL_TYPE HLDIR   (const char*, int, 
        const char*, int);
  void HBOOK_CALL_TYPE HCDIR(const char*, int, const char*, int);
  void HBOOK_CALL_TYPE HMDIR(const char*, int, const char*, int);
}

long RWNT_BOOK ( const NTUPLEINFO& i )  {
  return ::RWNTBOOK(i.id, 
        i.numVar, 
        i.name, sizeof(i.name[0]), 
        i.rzdir, strlen(i.rzdir), 
        i.title, strlen(i.title));
}

long RWNT_INFO ( NTUPLEINFO& i ) {
  long r=::RWNTINFO(i.id, i.numVar, i.numEnt, i.name, sizeof(i.name[0]), i.frange, i.title, sizeof(i.title));
  for ( long j = 0; j < i.numVar; j++ )   {
    i.size[j] = sizeof(float)*CHAR_BIT;
    i.dim[j] = 0;
    i.hasRange[j] = 0;
    i.hasIndex[j] = 0;
  }
  memset(i.block,0,sizeof(i.block));
  return r;
}

long CWNT_BOOK ( const NTUPLEINFO& i )  {
  return ::CWNTBOOK(i.id, i.title, strlen(i.title)," ", 1);
}

long CWNT_INFO ( NTUPLEINFO& i ) {
  long r=::CWNTINFO(i.id,
                    i.name, sizeof(i.name[0]),
                    i.dim,
                    i.theDim,
                    i.hasIndex,
                    i.index, sizeof(i.index[0]), 
                    i.type, 1,
                    i.size,
                    i.hasRange,
                    i.irange,
                    i.frange,
                    i.title,sizeof(i.title)-1,
                    i.numVar,
                    i.numEnt,
		    i.block,sizeof(i.block[0])
		    );
  // Clean up ntuple block names
  for (long iv=0; iv<i.numVar; ++iv) {
    std::string blk = i.block[iv];
    blk = blk.substr(0,8);
    int ibs;
    if ( (ibs = blk.find(" ")) >= 0 ) {
      blk = blk.substr(0,ibs);
    }
    strcpy(i.block[iv],blk.c_str());
  }
  //  cleanForString(i.block, sizeof(i.block)-1);
  cleanForString(i.title, sizeof(i.title)-1);
  return r;
}

void HLNEXT(long& id, std::string& a, std::string& b, const std::string& c) {
  char buff[256], typ[2] = {0,0};
  memset(buff,0,sizeof(buff));
  ::HLNEXT(id, typ, sizeof(typ)-1, buff, sizeof(buff), c.data(), c.length());
  buff[sizeof(buff)-1] = 0;
  cleanForString(buff, sizeof(buff)-1);
  b = buff;
  a = typ;
}

void HCDIR(const std::string& a, const std::string& b )  {
  ::HCDIR(a.data(), a.length(), b.data(), b.length());
}

void HLDIR(const std::string& a, const std::string& b )  {
  ::HLDIR(a.data(), a.length(), b.data(), b.length());
}

void HMDIR(const std::string& a, const std::string& b )  {
  ::HMDIR(a.data(), a.length(), b.data(), b.length());
}

void HBNAME (const long& id, const std::string& a, const void* opt, const std::string& b) {
  ::HBNAME(id, a.data(), a.length(), opt, b.data(), b.length());
}

void HROPEN(const long& lun, const std::string& a, const std::string& b, const std::string& c, long& i, long& j) {
  ::HROPEN(lun, a.data(), a.length(), b.data(), b.length(), c.data(), c.length(), i, j);
}

#else
extern"C" {
 long HBOOK_CALL_TYPE rwntbook_(const long& id, 
                                const long& dim, 
                                const char tag[512][64],
                                const char* rzdir, 
                                const char* title, long, long, long);
 long HBOOK_CALL_TYPE rwntinfo_(const long& id, 
                                long& dim, 
                                long& ents, 
                                char tags[512][64],
                                double range[512][2], 
                                char title[], long, long);
 long HBOOK_CALL_TYPE cwntbook_(const long& id, 
                                const char title[],
                                const char opt[], long, long);
 long HBOOK_CALL_TYPE cwntinfo_(const long& id, 
                                char name[512][64],
                                long dim[], 
                                long theDim[512][5],
                                long hasIndex[], 
                                char index[512][64],
                                char type[], 
                                long size[], 
                                long hasRange[],
                                long irange[512][2], 
                                double frange[512][2], 
                                char title[], 
                                long& numVar, 
                                long& numEnt, 
				char block[512][12],
				long, long, long, long, long, long);
  void HBOOK_CALL_TYPE hlnext_(long& id, char*, char*, const char*, int, int, int);
  void hbname_ (const long& id, const char* title, const void* opt, const char* form, int, int);
  void hropen_(const long&, const char*, const char*, const char*, long&, long&, int,int,int);
  void hcdir_(const char*, const char*, int, int);
  void hldir_(const char*, const char*, int, int);
  void hmdir_(const char*, const char*, int, int);
}

long RWNT_BOOK(const NTUPLEINFO& i)  {
  return rwntbook_(i.id, 
       i.numVar, 
       i.name, 
       i.rzdir, 
       i.title, 
       sizeof(i.name[0]), strlen(i.rzdir), strlen(i.title));
}

long RWNT_INFO(NTUPLEINFO& i) {
  long r = rwntinfo_(i.id,
         i.numVar, 
         i.numEnt, 
         i.name, 
         i.frange, 
         i.title, 
         sizeof(i.name[0]), sizeof(i.title));
  for ( long j = 0; j < i.numVar; j++ )   {
    i.size[j] = sizeof(float)*CHAR_BIT;
    i.dim[j] = 0;
    i.hasRange[j] = 0;
    i.hasIndex[j] = 0;
  }
  memset(i.block,0,sizeof(i.block));
  return r;
}

long CWNT_BOOK(const NTUPLEINFO& i)  {
  return cwntbook_(i.id, i.title, " ", strlen(i.title), 1);
}

long CWNT_INFO(NTUPLEINFO& i) {
  long r = cwntinfo_(i.id,
		     i.name,
		     i.dim,
		     i.theDim,
		     i.hasIndex,
		     i.index,
		     i.type,
		     i.size,
		     i.hasRange,
		     i.irange,
		     i.frange,
		     i.title,
		     i.numVar,
		     i.numEnt,
		     i.block,
		     sizeof(i.name[0]), sizeof(i.index[0]), 1,
		     sizeof(i.title)-1, 
		     sizeof(i.block[0]),sizeof(i.index[0])
		     );

  // Clean up ntuple block names
  for (long iv=0; iv<i.numVar; ++iv) {
    std::string blk = i.block[iv];
    blk = blk.substr(0,8);
    int ibs;
    if ( (ibs = blk.find(" ")) >= 0 ) {
      blk = blk.substr(0,ibs);
    }
    strcpy(i.block[iv],blk.c_str());
  }
    
  cleanForString(i.title, sizeof(i.title)-1);
  return r;
}

void HLNEXT(long& id, std::string& a, std::string& b, const std::string& c) {
  char buff[256], typ[2] = {0,0};
  memset(buff,0,sizeof(buff));
  hlnext_(id, typ, buff, c.c_str(), sizeof(typ)-1, sizeof(buff), c.length());
  buff[sizeof(buff)-1] = 0;
  cleanForString(buff, sizeof(buff)-1);
  b = buff;
  a = typ;
}

void HCDIR(const std::string& a, const std::string& b )  {
  hcdir_(a.c_str(), b.c_str(), a.length(), b.length());
}
void HLDIR(const std::string& a, const std::string& b )  {
  hldir_(a.c_str(), b.c_str(), a.length(), b.length());
}
void HMDIR(const std::string& a, const std::string& b )  {
  hmdir_(a.c_str(), b.c_str(), a.length(), b.length());
}
void HBNAME (const long& id, const std::string& a, const void* opt, const std::string& b) {
  hbname_(id, a.c_str(), opt, b.c_str(), a.length(), b.length());
}
void HROPEN(const long& lun, const std::string& a, const std::string& b, const std::string& c, long& i, long& j) {
  hropen_(lun, a.c_str(), b.c_str(), c.c_str(), i, j, a.length(), b.length(), c.length());
}
#endif
