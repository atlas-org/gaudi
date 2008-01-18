//------------------------------------------------------------------------------
//
// Implementation of class :  HbookCnv::H2DCnv
//
// Author :                   Pavel Binko
//
//------------------------------------------------------------------------------
// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/HbookCnv/src/H2DCnv.cpp,v 1.14 2006/01/10 20:11:14 hmd Exp $
#define HBOOKCNV_H2DCNV_CPP

// Include files
#include <iostream>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "AIDA/IHistogramFactory.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IAxis.h"
#include "HbookDef.h"
#include "H2DCnv.h"

namespace HbookCnv {
  using AIDA::IHistogramFactory;
  using AIDA::IHistogram2D;
  using AIDA::IAxis;
}

// Instantiation of a static factory class used by clients to create
// instances of this service
DECLARE_NAMESPACE_CONVERTER_FACTORY(HbookCnv,H2DCnv)

/// Book 2 D histogram
StatusCode HbookCnv::H2DCnv::book(IOpaqueAddress* pAdd, DataObject* pObj){
  IHistogram2D* h = dynamic_cast<IHistogram2D*>(pObj);
  if ( 0 != h ) {
    int idh = pAdd->ipar()[0];
    const IAxis& x = h->xAxis();
    const IAxis& y = h->yAxis();
    ::HBOOK2( idh, h->title(),
              x.bins(), float(x.lowerEdge()), float(x.upperEdge()),
              y.bins(), float(y.lowerEdge()), float(y.upperEdge()));
    ::HIDOPT(idh, "STAT");

    if ( !x.isFixedBinning() || !y.isFixedBinning() ) {
      MsgStream log( msgSvc(), "HbookCnv::H2DCnv" );
      log << MSG::WARNING 
          << "2D variable bin size histograms not supported in HBOOK" << endreq
          << "  -> Persistent 2D histogram " << idh << " '" + h->title() + "' INACCURATE!"
          << endreq;
    }

    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

/// Convert the transient object to the requested representation.
StatusCode HbookCnv::H2DCnv::updateRep(IOpaqueAddress* pAddr,
                                       DataObject* pObj)  {
  IHistogram2D* h = dynamic_cast<IHistogram2D*>(pObj);
  if ( 0 != h ) {
    const IAxis& x = h->xAxis();
    int    histoID  = pAddr->ipar()[0]; // Histogram ID
    int    nBinX    = x.bins();         // Number of bins in the axis X
    double xLow     = x.lowerEdge();    // X-lower edge
    double xHigh    = x.upperEdge();    // X-upper edge
    double x_uflow  = xLow  - 1.0;      // Underflow coordinate X
    double x_oflow  = xHigh + 1.0;      // Overflow coordinate X

    const IAxis& y = h->yAxis();
    int    nBinY    = y.bins();         // Number of bins in the axis Y
    double yLow     = y.lowerEdge();    // Y-lower edge
    double yHigh    = y.upperEdge();    // Y-upper edge
    double y_uflow  = yLow  - 1.0;      // Underflow coordinate Y
    double y_oflow  = yHigh + 1.0;      // Overflow coordinate Y

    int    entries  = 0;                // Number of entries in a bin
    double height   = 0.;               // Height if a bin
    double centreX  = 0.;               // Centre of a bin on the axis X
    double centreY  = 0.;               // Centre of a bin on the axis Y
    double weight   = 0.;               // height / entries
    int    ix       = 0;                // Bin index on the axis X
    int    iy       = 0;                // Bin index on the axis Y
    int    j        = 0;                // Fill index

    double* errs     = new double[nBinX*nBinY];  // Array of bin errors
    int    ind      = 0;                       // Index of bin errors

    double x_mean  = xLow + (xHigh-xLow)/2.0;
    double y_mean  = yLow + (yHigh-yLow)/2.0;

    ::HRESET(histoID, " ", 1);

    // Fill the HBOOK 2D histogram with fixed binning
    for ( ix = 0; ix < nBinX; ix++ )  {
      for ( iy = 0; iy < nBinY; iy++, ind++ )  {
        errs[ind] = h->binError(ix,iy);
        height = h->binHeight(ix,iy);
        if( 0 != height ) {
          centreX = h->binMeanX( ix, iy );
          centreY = h->binMeanY( ix, iy );
          entries = h->binEntries(ix,iy);
          weight  = height / entries;
          for( j = 0; j < entries; j++ ) {
            ::HFILL(histoID, (float)centreX, (float)centreY, (float)weight);
          }
        }
      }
    }
    ::HPAKE(histoID,errs);
    delete [] errs;

    // Underflow and overflow bins
    // W
    entries          = h->binEntriesX( IAxis::UNDERFLOW_BIN ) - 
                       h->binEntries ( IAxis::UNDERFLOW_BIN,  IAxis::OVERFLOW_BIN ) - 
                       h->binEntries ( IAxis::UNDERFLOW_BIN, IAxis::UNDERFLOW_BIN );
    height           = h->binHeightX ( IAxis::UNDERFLOW_BIN ) - 
                       h->binHeight  ( IAxis::UNDERFLOW_BIN, IAxis::OVERFLOW_BIN ) -
                       h->binHeight  ( IAxis::UNDERFLOW_BIN, IAxis::UNDERFLOW_BIN );
    weight  = height / entries;
    for( j = 0; j < entries; j++ ) {
      ::HFILL(histoID, (float)x_uflow, (float)y_mean, (float)weight);
    }
    // NW
    entries          = h->binEntries ( IAxis::UNDERFLOW_BIN, IAxis::OVERFLOW_BIN  );
    height           = h->binHeight  ( IAxis::UNDERFLOW_BIN, IAxis::OVERFLOW_BIN  );
    weight  = height / entries;
    for( j = 0; j < entries; j++ ) {
      ::HFILL(histoID, (float)x_uflow, (float)y_oflow, (float)weight);
    }
    // N
    entries          = h->binEntriesY( IAxis::OVERFLOW_BIN  ) - 
                       h->binEntries ( IAxis::UNDERFLOW_BIN, IAxis::OVERFLOW_BIN  ) -
                       h->binEntries ( IAxis::OVERFLOW_BIN,  IAxis::OVERFLOW_BIN  );
    height           = h->binHeightY ( IAxis::OVERFLOW_BIN  ) -
                       h->binHeight  ( IAxis::UNDERFLOW_BIN, IAxis::OVERFLOW_BIN  ) -
                       h->binHeight  ( IAxis::OVERFLOW_BIN,  IAxis::OVERFLOW_BIN  );
    weight  = height / entries;
    for( j = 0; j < entries; j++ ) {
      ::HFILL(histoID, (float)x_mean, (float)y_oflow, (float)weight);
    }
    // NE
    entries          = h->binEntries ( IAxis::OVERFLOW_BIN,  IAxis::OVERFLOW_BIN  );
    height           = h->binHeight  ( IAxis::OVERFLOW_BIN,  IAxis::OVERFLOW_BIN  );
    weight  = height / entries;
    for( j = 0; j < entries; j++ ) {
      ::HFILL(histoID, (float)x_oflow, (float)y_oflow, (float)weight);
    }
    // E
    entries          = h->binEntriesX( IAxis::OVERFLOW_BIN  ) - 
                       h->binEntries ( IAxis::OVERFLOW_BIN, IAxis::UNDERFLOW_BIN  ) -
                       h->binEntries ( IAxis::OVERFLOW_BIN, IAxis::OVERFLOW_BIN  );
    height           = h->binHeightX ( IAxis::OVERFLOW_BIN  ) -
                       h->binHeight  ( IAxis::OVERFLOW_BIN, IAxis::UNDERFLOW_BIN  ) -
                       h->binHeight  ( IAxis::OVERFLOW_BIN, IAxis::OVERFLOW_BIN  );
    weight  = height / entries;
    for( j = 0; j < entries; j++ ) {
      ::HFILL(histoID, (float)x_oflow, (float)y_mean, (float)weight);
    }
    // SE
    entries          = h->binEntries ( IAxis::OVERFLOW_BIN,  IAxis::UNDERFLOW_BIN );
    height           = h->binHeight  ( IAxis::OVERFLOW_BIN,  IAxis::UNDERFLOW_BIN );
    weight  = height / entries;
    for( j = 0; j < entries; j++ ) {
      ::HFILL(histoID, (float)x_oflow, (float)y_uflow, (float)weight);
    }
    // S
    entries          = h->binEntriesY( IAxis::UNDERFLOW_BIN  ) - 
                       h->binEntries ( IAxis::OVERFLOW_BIN,  IAxis::UNDERFLOW_BIN  ) -
                       h->binEntries ( IAxis::UNDERFLOW_BIN, IAxis::UNDERFLOW_BIN  );
    height           = h->binHeightY ( IAxis::UNDERFLOW_BIN  ) -
                       h->binHeight  ( IAxis::OVERFLOW_BIN,  IAxis::UNDERFLOW_BIN  ) -
                       h->binHeight  ( IAxis::UNDERFLOW_BIN, IAxis::UNDERFLOW_BIN  );
    weight  = height / entries;
    for( j = 0; j < entries; j++ ) {
      ::HFILL(histoID, (float)x_mean, (float)y_uflow, (float)weight);
    }
    // SW
    entries          = h->binEntries ( IAxis::UNDERFLOW_BIN, IAxis::UNDERFLOW_BIN );
    height           = h->binHeight  ( IAxis::UNDERFLOW_BIN, IAxis::UNDERFLOW_BIN );
    weight  = height / entries;
    for( j = 0; j < entries; j++ ) {
      ::HFILL(histoID, (float)x_uflow, (float)y_uflow, (float)weight);
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

/// Create 2 D histogram from disk representation
StatusCode HbookCnv::H2DCnv::createObj(IOpaqueAddress* pAddr,
                                       DataObject*& refpObj)
{
  //MsgStream log(msgSvc(), "HHistoCnv");
  IHistogramFactory* pFac = dynamic_cast<IHistogramFactory*>(dataProvider());
  if ( 0 != pAddr && pFac ) {
    std::string loc = pAddr->registry()->identifier();
    long id = pAddr->ipar()[0];
    if ( readObject(loc, id).isSuccess() )  {
      std::string title;
      int nx, ny, nwt, lpaw;
      float xmi, xma, ymi, yma;
      ::HGIVE(id, title, nx, xmi, xma, ny, ymi, yma, nwt, lpaw);
      pAddr->addRef();
      refpObj = dynamic_cast<DataObject*>(pFac->createHistogram2D(loc, title,
                                                                  nx, xmi, xma,
                                                                  ny, ymi, yma));
      refpObj->registry()->setAddress(pAddr);
      StatusCode sc = updateObj(pAddr, refpObj);
      pAddr->release();
      return sc;
    }
  }
  return StatusCode::FAILURE;
}

/// Convert the transient object to the requested representation.
StatusCode HbookCnv::H2DCnv::updateObj(IOpaqueAddress* pAddr,
                                       DataObject* pObj)  {
  IHistogram2D* h = dynamic_cast<IHistogram2D*>(pObj);

  if ( 0 != h && 0 != pAddr ) {
    int    id  = pAddr->ipar()[0]; // Histogram ID
    std::string loc = pAddr->registry()->identifier();
    if ( !::HEXIST( id ) )  {
      readObject(loc, id);
    }
    if ( ::HEXIST( id ) )  {
      const  IAxis& x = h->xAxis();
      int    nBinX    = x.bins();         // Number of bins
      double xLow     = x.lowerEdge();    // Histogram lower edge
      double xHigh    = x.upperEdge();    // Histogram upper edge
      double x_uflow  = xLow  - 1.0;      // Underflow coordinate
      double x_oflow  = xHigh + 1.0;      // Overflow coordinate

      const  IAxis& y = h->yAxis();
      int    nBinY    = y.bins();         // Number of bins in the axis Y
      double yLow     = y.lowerEdge();    // Y-lower edge
      double yHigh    = y.upperEdge();    // Y-upper edge
      double y_uflow  = yLow  - 1.0;      // Underflow coordinate Y
      double y_oflow  = yHigh + 1.0;      // Overflow coordinate Y
      // float x_mean  = xLow + (xHigh-xLow)/2.0;
      // float y_mean  = yLow + (yHigh-yLow)/2.0;
      double centreX, centreY, height, err;
      int ix, iy;

      h->reset();

      for ( ix = 0; ix < nBinX; ix++ )  {
        for ( iy = 0; iy < nBinY; iy++ )  {
          centreX = h->binMeanX(ix,iy);
          centreY = h->binMeanY(ix,iy);
          height  = ::HIJ(id, ix+1, iy+1);
          err     = ::HIJE(id, ix+1, iy+1);
          // Unfortunately IHistogram2D does not support to set bins
          // and bin errors....errors will be SQRT(height)
          h->fill(centreX, centreY, height);
        }
      }

      // Underflow and overflow bins
      // W
      for ( iy = 0; iy < nBinY; iy++ )  {
        centreY = h->binMeanY(IAxis::UNDERFLOW_BIN,iy);
        height  = ::HIJ(id, 0, iy+1);
        h->fill(x_uflow, centreY, height);
      }
      // NW
      height  = ::HIJ(id, 0, nBinY+1);
      h->fill(x_uflow, y_oflow, height);
      // N
      for ( ix = 0; ix < nBinX; ix++ )  {
        double centreX = h->binMeanX(ix,IAxis::OVERFLOW_BIN);
        double height  = ::HIJ(id, ix+1, nBinY+1);
        h->fill(centreX, y_oflow, height);
      }
      // NE
      height  = ::HIJ(id, nBinX+1, nBinY+1);
      h->fill(x_oflow, y_oflow, height);
      // E
      for ( iy = 0; iy < nBinY; iy++ )  {
        double centreY = h->binMeanY(IAxis::OVERFLOW_BIN,iy);
        double height  = ::HIJ(id, nBinX+1, iy+1);
        h->fill(x_oflow, centreY, height);
      }
      // SE
      height  = ::HIJ(id, nBinX+1, 0);
      h->fill(x_oflow, y_uflow, height);
      // S
      for ( ix = 0; ix < nBinX; ix++ )  {
        double centreX = h->binMeanX(ix,IAxis::UNDERFLOW_BIN);
        double height  = ::HIJ(id, ix+1, 0);
        h->fill(centreX, y_uflow, height);
      }
      // SW
      height  = ::HIJ(id, 0, 0);
      h->fill(x_uflow, y_uflow, height);

      ::HDELET(id);
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}
