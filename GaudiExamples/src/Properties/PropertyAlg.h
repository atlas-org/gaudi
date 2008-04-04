// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiExamples/src/Properties/PropertyAlg.h,v 1.5 2006/12/04 17:15:11 mato Exp $
#ifndef GAUDIEXAMPLE_PROPERTYALG_H
#define GAUDIEXAMPLE_PROPERTYALG_H

// Include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"

template <class T> class AlgFactory;

/** @class PropertyAlg
    Trivial Algorithm for tutotial purposes
    
    @author nobody
*/
class PropertyAlg : public Algorithm {
protected:
  /// Constructor of this form must be provided
  PropertyAlg(const std::string& name, ISvcLocator* pSvcLocator); 
public:
  friend class AlgFactory<PropertyAlg>;
  /// Three mandatory member functions of any algorithm
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  /// Callbacks for properties
  void readHandler(Property&);
  void updateHandler(Property&);

private:
  /// These data members are used in the execution of this algorithm
  /// They are set in the initialisation phase by the job options service

  int         m_int;
  double      m_double;
  std::string m_string;
  bool        m_bool;

  std::vector<int>         m_intarray;
  std::vector<double>      m_doublearray;
  std::vector<std::string> m_stringarray;
  std::vector<bool>        m_boolarray;
  std::vector<double>      m_emptyarray;
  std::vector<double>      u_doublearrayunits;
  std::vector<double>      u_doublearray;

  IntegerProperty  p_int;
  DoubleProperty   p_double;
  StringProperty   p_string;
  BooleanProperty  p_bool;

  IntegerArrayProperty  p_intarray;
  DoubleArrayProperty   p_doublearray;
  StringArrayProperty   p_stringarray;
  BooleanArrayProperty  p_boolarray;
};

#endif    // GAUDIEXAMPLE_PROPERTYALG_H
