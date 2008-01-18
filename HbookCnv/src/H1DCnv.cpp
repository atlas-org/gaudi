// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/HbookCnv/src/H1DCnv.cpp,v 1.13 2006/01/10 20:11:13 hmd Exp $
#define HBOOKCNV_H1DCNV_CPP


// Include files
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"
#include "AIDA/IHistogramFactory.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IAxis.h"
#include "HbookDef.h"
#include "H1DCnv.h"
#include <iostream>

namespace HbookCnv {
  using AIDA::IHistogramFactory;
  using AIDA::IHistogram1D;
  using AIDA::IAxis;
}

//------------------------------------------------------------------------------
//
// Implementation of class :  HbookCnv::H1DCnv
//
// Author :                   Pavel Binko
// Author :                   Grigori Rybkine
//
//------------------------------------------------------------------------------


// Instantiation of a static factory class used by clients to create
// instances of this service
DECLARE_NAMESPACE_CONVERTER_FACTORY(HbookCnv,H1DCnv)

/// Book 1 D histogram
StatusCode HbookCnv::H1DCnv::book(IOpaqueAddress* pAdd, DataObject* pObj){
  IHistogram1D* h = dynamic_cast<IHistogram1D*>(pObj);
  if ( 0 != h ) {
    int idh = pAdd->ipar()[0];
    const IAxis& x = h->axis();
    int nBinX = x.bins();      // Number of bins in the axis X

    if ( x.isFixedBinning() ) {
      ::HBOOK1(idh, h->title(), nBinX, float(x.lowerEdge()), float(x.upperEdge())); 
      ::HIDOPT(idh, "STAT");
    }
    else {
      float *edges = new float[nBinX+1];
      for ( int i = 0; i < nBinX; i++ )
        edges[i] = float(x.binLowerEdge(i));

      edges[nBinX] = float(x.upperEdge());
      ::HBOOKB(idh, h->title(), nBinX, edges);
      delete [] edges;
    }
    
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

/// Convert the transient object to the requested representation.
StatusCode HbookCnv::H1DCnv::updateRep(IOpaqueAddress* pAddr, 
                                       DataObject* pObj)  {
  IHistogram1D* h = dynamic_cast<IHistogram1D*>(pObj);
  if ( 0 != h ) {
    const  IAxis& x = h->axis();
    int    histoID  = pAddr->ipar()[0]; // Histogram ID
    int    nBinX    = x.bins();         // Number of bins
    float  xLow     = float(x.lowerEdge());    // Histogram lower edge
    float  xHigh    = float(x.upperEdge());    // Histogram upper edge
    float  x_uflow  = xLow  - 1.0F;      // Underflow coordinate
    float  x_oflow  = xHigh + 1.0F;      // Overflow coordinate

    int    entries  = 0;                // Number of entries in a bin
    float  height   = 0.;               // Height if a bin
    float  centre   = 0.;               // Mean of a bin
    float  weight   = 0.;               // height / entries
    float *errs     = new float[nBinX]; // Array of bin errors
    int    i, j     = 0;

    // Fill the HBOOK 1D histogram (with fixed or variable binning)
    for ( i = 0; i < nBinX; i++ )  {
      errs[i] = float(h->binError(i));
      height  = float(h->binHeight(i));
      if( 0 != height ) {
        centre  = float(h->binMean(i));
        entries = h->binEntries(i);
        weight  = height / entries;
        for( j = 0; j < entries; j++ ) {
          ::HFILL(histoID, centre, 0.0, weight);
        }
      }
    }
    ::HPAKE(histoID,errs);
    delete [] errs;


    // Underflow bins
    entries          = h->binEntries( AIDA::IAxis::UNDERFLOW_BIN );
    height           = float(h->binHeight( AIDA::IAxis::UNDERFLOW_BIN ));
    weight  = height / entries;
    for( j = 0; j < entries; j++ ) {
      ::HFILL(histoID, x_uflow, 0.0, weight);
    }
    // Overflow bins
    entries          = h->binEntries( AIDA::IAxis::OVERFLOW_BIN );
    height           = float(h->binHeight( AIDA::IAxis::OVERFLOW_BIN ));
    weight  = height / entries;
    for( j = 0; j < entries; j++ ) {
      ::HFILL(histoID, x_oflow, 0.0, weight);
    }

    // Set the correct MEAN and RMS vales in the HBOOK histogram
    float mean          = (float) h->mean();
    float rms           = (float) h->rms();
    float sumOfHeights  = (float) h->sumBinHeights();
    float equivEntries  = (float) h->equivalentBinEntries();
    ::SET_MEAN_RMS( histoID, mean, rms, sumOfHeights, equivEntries );
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

/// Create 1 D histogram from disk representation
StatusCode HbookCnv::H1DCnv::createObj(IOpaqueAddress* pAddr, 
                                       DataObject*& refpObj)
{
  //MsgStream log(msgSvc(), "HHistoCnv");

  IHistogramFactory* pFac = dynamic_cast<IHistogramFactory*>(dataProvider());
  if ( 0 != pAddr && pFac ) {
    std::string loc = pAddr->registry()->identifier();
    long id = pAddr->ipar()[0];
    if ( readObject(loc, id).isSuccess() )  {
      std::string tit;
      int nx, ny, nwt, lpaw;
      float xmi, xma, ymi, yma;
      ::HGIVE(id, tit, nx, xmi, xma, ny, ymi, yma, nwt, lpaw);
      long kind[32];
      ::HKIND(id, kind, "A", 1);

      if ( 0 == kind[5] ) {
        pAddr->addRef();
        refpObj = dynamic_cast<DataObject*>(pFac->createHistogram1D(loc,tit,
								    nx,xmi,xma));
      }
      else {
        std::vector<double> binEdges;
        float val;
        for ( int i = 1; i <= nx; ++i ) {
          ::HIX(id, i, val);
          binEdges.push_back(val);
        }
        binEdges.push_back(xma);
        pAddr->addRef();
        refpObj = dynamic_cast<DataObject*>(pFac->createHistogram1D(loc,tit,
								    binEdges));
      }
      
      refpObj->registry()->setAddress(pAddr);
      StatusCode sc = updateObj(pAddr, refpObj);
      pAddr->release();
      return sc;
    }
  }
  return StatusCode::FAILURE;
}

/// Convert the transient object to the requested representation.
StatusCode HbookCnv::H1DCnv::updateObj(IOpaqueAddress* pAddr, 
                                       DataObject* pObj)  {
  IHistogram1D* h = dynamic_cast<IHistogram1D*>(pObj);
  if ( 0 != h && 0 != pAddr ) {
    int    id  = pAddr->ipar()[0]; // Histogram ID
    std::string loc = pAddr->registry()->identifier();
    if ( !::HEXIST( id ) )  {
      readObject(loc, id);
    }
    if ( ::HEXIST( id ) )  {
      const  IAxis& x = h->axis();
      int    nBinX    = x.bins();         // Number of bins
      double xLow     = x.lowerEdge();    // Histogram lower edge
      double xHigh    = x.upperEdge();    // Histogram upper edge
      double x_uflow  = xLow  - 1.0;      // Underflow coordinate
      double x_oflow  = xHigh + 1.0;      // Overflow coordinate
      double height = 0, err = 0, mean = 0, rms = 0, 
            centre = 0, sumOfHeights = 0, equivEntries = 0;
      h->reset();
      for ( int i = 0; i < nBinX; i++ )  {
        centre = h->binMean(i);
        height = ::HI (id, i+1);
        err    = ::HIE(id, i+1);
        sumOfHeights += height;
        // Unfortunately IHistogram1D does not support to set bins
        // and bin errors....errors will be SQRT(height)
        h->fill(centre, height);
      }
      // Underflow bins: also here no error filling possible
    //centre = h->binMean(AIDA::IAxis::UNDERFLOW_BIN);
      height = ::HI(id, 0);
      sumOfHeights += height;
      h->fill(x_uflow, height);
      // Overflow bins: also here no error filling possible
    //centre = h->binMean(AIDA::IAxis::OVERFLOW_BIN);
      height = ::HI(id, nBinX+1);
      sumOfHeights += height;
      h->fill(x_oflow, height);
      // Set the correct MEAN and RMS vales in the HBOOK histogram
      mean = ::HSTATI(id, 1, " ", 0);
      rms  = ::HSTATI(id, 2, " ", 0);
      equivEntries = ::HSTATI(id, 3, " ", 0);
      //h->SET_MEAN_RMS( id, mean, rms, sumOfHeights, equivEntries );
      ::HDELET(id);
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}
