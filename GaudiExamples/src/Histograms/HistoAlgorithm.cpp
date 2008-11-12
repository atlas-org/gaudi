// $Id: HistoAlgorithm.cpp,v 1.11 2006/11/30 10:35:27 mato Exp $


// Include files 
#include "HistoAlgorithm.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataObject.h"

#include <math.h>

DECLARE_ALGORITHM_FACTORY(HistoAlgorithm);


//------------------------------------------------------------------------------
HistoAlgorithm::HistoAlgorithm(const std::string& name,
							   ISvcLocator* pSvcLocator) :
							   Algorithm(name, pSvcLocator)
//------------------------------------------------------------------------------
{
  m_h1D = m_h1DVar = 0;
  m_h2D = m_h2DVar = 0;
  m_h3D = m_h3DVar = 0;
  m_p1D = 0;
  m_p2D = 0;
}


//------------------------------------------------------------------------------
StatusCode HistoAlgorithm::initialize()
//------------------------------------------------------------------------------
{
  MsgStream log( msgSvc(), name() );
  int i;
  std::vector<double> edges;
  for( i = 0; i <= 10; i++) edges.push_back( double(i)*double(i) );
  // Getting existing histograms
  SmartDataPtr<IHistogram1D> h1(histoSvc(),"InFile/1");
  if( h1 ) {
    log << MSG::INFO << "Got histogram from file" << endreq;
    histoSvc()->print( h1);
  } else {
    log << MSG::ERROR << "Histogram h1 not found" << endreq;
  }
  
  log << MSG::INFO << "Booking Histograms" << endreq;

  // Book 1D histogram with fixed and variable binning
  m_h1D    = histoSvc()->book( "1", "1D fix binning", 50, 0., 100. );
  m_h1DVar = histoSvc()->book( "Var/1", "1D variable binning", edges );

  m_h2D    = histoSvc()->book( "2", "2D fix binning", 50, 0., 100.,
                                                      50, 0., 100.  );
  m_h2DVar = histoSvc()->book( "Var/2", "2D variable binning", edges, edges );

  m_h3D    = histoSvc()->book( "3", "3D fix binning", 50, 0., 100.,
                                                      50, 0., 100.,
                                                      10, 0., 100.  );
  m_h3DVar = histoSvc()->book( "Var/3", "3D variable binning", edges,
                                                               edges,
                                                               edges );
  m_p1D    = histoSvc()->bookProf( "Prof/1", "Profile 1D", 50, 0., 100. );
  m_p2D    = histoSvc()->bookProf( "Prof/2", "Profile 2D", 50, 0., 100.,
                                                           50, 0., 100. );
                                                               
  if( 0 == m_h1D ||0 == m_h2D || 0 == m_h3D ||
      0 == m_h1DVar ||0 == m_h2DVar ||0 == m_h3DVar ||
      0 == m_p1D || 0 == m_p2D  ) { 
    log << MSG::ERROR << "----- Cannot book or register histograms -----" << endreq;
    return StatusCode::FAILURE;  
  }
  log << MSG::INFO << "Finished booking Histograms" << endreq;

  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode HistoAlgorithm::execute()
//------------------------------------------------------------------------------
{
  MsgStream log( msgSvc(), name() );

  static int n = 0;

  double x = sin(double(n)) * 52. + 50.;
  double y = cos(double(2*n)) * 52. + 50.;
  double z = cos(double(5*n)) * 52. + 50.;

  m_h1D->fill(x);
  m_h1DVar->fill(x);

  m_h2D->fill(x, y);
  m_h2DVar->fill(x, y);

  m_h3D->fill(x, y, z);
  m_h3DVar->fill(x, y, z);

  m_p1D->fill(x, y);
  m_p2D->fill(x, y, z);

  n++;
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode HistoAlgorithm::finalize()
//------------------------------------------------------------------------------
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Finalizing..." << endreq;

  //1D HISTOGRAM STUFF
  log << MSG::DEBUG << "Info about Histogram 1D\n" << endreq;
  //General Info:
	log << MSG::DEBUG << "title: " << m_h1D->title() <<  endreq;
	log << MSG::DEBUG << "dim:   " << m_h1D->dimension() << endreq;
  //Histo Info:
	log << MSG::DEBUG << "Entries (In): "  << m_h1D->entries()  << endreq;
	log << MSG::DEBUG << "Entries (Out): " << m_h1D->extraEntries()  << endreq;
	log << MSG::DEBUG << "Entries (All): " << m_h1D->allEntries()  << endreq;
	log << MSG::DEBUG << "Entries (Eq.): " << m_h1D->equivalentBinEntries()  << endreq;

	log << MSG::DEBUG << "Sum of Heights(In): "  << m_h1D->sumBinHeights()  << endreq;
	log << MSG::DEBUG << "Sum of Heights(Out): " << m_h1D->sumExtraBinHeights()  << endreq;
	log << MSG::DEBUG << "Sum of Heights(All): " << m_h1D->sumAllBinHeights() << endreq;
	log << MSG::DEBUG << "Min. Heights(In): "    << m_h1D->minBinHeight()  << endreq;
	log << MSG::DEBUG << "Max. Heights(In): "    << m_h1D->maxBinHeight()  << endreq;
  //Statistics:
	log << MSG::DEBUG << "mean: " << m_h1D->mean() << endreq;
	log << MSG::DEBUG << "rms: " <<  m_h1D->rms()  << endreq;
  //Print Histogram stream in ASCII on screen:	
	log << MSG::DEBUG << "Printing H1: " << endreq;
	//histoSvc()->print( m_h1D );

  //1DVar HISTOGRAM STUFF
  log << MSG::DEBUG << "Info about Histogram 1DVar\n" << endreq;
  //General Info:
	log << MSG::DEBUG << "title: " << m_h1DVar->title() <<  endreq;
	log << MSG::DEBUG << "dim:   " << m_h1DVar->dimension() << endreq;
  //Histo Info:
	log << MSG::DEBUG << "Entries (In): "  << m_h1DVar->entries()  << endreq;
	log << MSG::DEBUG << "Entries (Out): " << m_h1DVar->extraEntries()  << endreq;
	log << MSG::DEBUG << "Entries (All): " << m_h1DVar->allEntries()  << endreq;
	log << MSG::DEBUG << "Entries (Eq.): " << m_h1DVar->equivalentBinEntries()  << endreq;
	log << MSG::DEBUG << "Sum of Heights(In): "  << m_h1DVar->sumBinHeights()  << endreq;
	log << MSG::DEBUG << "Sum of Heights(Out): " << m_h1DVar->sumExtraBinHeights()  << endreq;
	log << MSG::DEBUG << "Sum of Heights(All): " << m_h1DVar->sumAllBinHeights() << endreq;
	log << MSG::DEBUG << "Min. Heights(In): "    << m_h1DVar->minBinHeight()  << endreq;
	log << MSG::DEBUG << "Max. Heights(In): "    << m_h1DVar->maxBinHeight()  << endreq;
  //Statistics:
	log << MSG::DEBUG << "mean: " << m_h1DVar->mean() << endreq;
	log << MSG::DEBUG << "rms: " <<  m_h1DVar->rms()  << endreq;
  //Print Histogram stream in ASCII on screen:	
	log << MSG::DEBUG << "Printing H1Var: " << endreq;
	//histoSvc()->print( m_h1DVar );

  //2D HISTOGRAM STUFF
  log << MSG::DEBUG << "Info about Histogram 2D\n" << endreq;
  //General Info:
	log << MSG::DEBUG << "title: " << m_h2D->title() <<  endreq;
	log << MSG::DEBUG << "dim:   " << m_h2D->dimension() << endreq;
  //Histo Info:
	log << MSG::DEBUG << "Entries (In): "  << m_h2D->entries()  << endreq;
	log << MSG::DEBUG << "Entries (Out): " << m_h2D->extraEntries()  << endreq;
	log << MSG::DEBUG << "Entries (All): " << m_h2D->allEntries()  << endreq;
	log << MSG::DEBUG << "Entries (Eq.): " << m_h2D->equivalentBinEntries()  << endreq;
	log << MSG::DEBUG << "Sum of Heights(In): "  << m_h2D->sumBinHeights()  << endreq;
	log << MSG::DEBUG << "Sum of Heights(Out): " << m_h2D->sumExtraBinHeights()  << endreq;
	log << MSG::DEBUG << "Sum of Heights(All): " << m_h2D->sumAllBinHeights() << endreq;
	log << MSG::DEBUG << "Min. Heights(In): "    << m_h2D->minBinHeight()  << endreq;
	log << MSG::DEBUG << "Max. Heights(In): "    << m_h2D->maxBinHeight()  << endreq;
  //Statistics:
	log << MSG::DEBUG << "meanX: " << m_h2D->meanX() << endreq;
	log << MSG::DEBUG << "meanY: " << m_h2D->meanY() << endreq;
	log << MSG::DEBUG << "rmsX: " <<  m_h2D->rmsX()  << endreq;
	log << MSG::DEBUG << "rmsY: " <<  m_h2D->rmsY()  << endreq;
  //Print Histogram stream in ASCII on screen:	
	log << MSG::DEBUG << "Printing H1Var: " << endreq;
	//histoSvc()->print( m_h2D );

  //2DVar HISTOGRAM STUFF
  log << MSG::DEBUG << "Info about Histogram 2DVar\n" << endreq;
  //General Info:
	log << MSG::DEBUG << "title: " << m_h2DVar->title() <<  endreq;
	log << MSG::DEBUG << "dim:   " << m_h2DVar->dimension() << endreq;
  //Histo Info:
	log << MSG::DEBUG << "Entries (In): "  << m_h2DVar->entries()  << endreq;
	log << MSG::DEBUG << "Entries (Out): " << m_h2DVar->extraEntries()  << endreq;
	log << MSG::DEBUG << "Entries (All): " << m_h2DVar->allEntries()  << endreq;
	log << MSG::DEBUG << "Entries (Eq.): " << m_h2DVar->equivalentBinEntries()  << endreq;

	log << MSG::DEBUG << "Sum of Heights(In): "  << m_h2DVar->sumBinHeights()  << endreq;
	log << MSG::DEBUG << "Sum of Heights(Out): " << m_h2DVar->sumExtraBinHeights()  << endreq;
	log << MSG::DEBUG << "Sum of Heights(All): " << m_h2DVar->sumAllBinHeights() << endreq;
	log << MSG::DEBUG << "Min. Heights(In): "    << m_h2DVar->minBinHeight()  << endreq;
	log << MSG::DEBUG << "Max. Heights(In): "    << m_h2DVar->maxBinHeight()  << endreq;
  //Statistics:
	log << MSG::DEBUG << "meanX: " << m_h2DVar->meanX() << endreq;
	log << MSG::DEBUG << "meanY: " << m_h2DVar->meanX() << endreq;
	log << MSG::DEBUG << "rmsX: " <<  m_h2DVar->rmsX()  << endreq;
	log << MSG::DEBUG << "rmsY: " <<  m_h2DVar->rmsY()  << endreq;
  //Print Histogram stream in ASCII on screen:	
//	log << MSG::DEBUG << "Printing H2Var: " << endreq;
//	histoSvc()->print( m_h2DVar );

  // Create X projection of the Pt versus P histogram m_hPtvsP
//   IHistogram1D* xProj  = histoSvc()->histogramFactory()->projectionX( "Proj/101",  *m_h2D );
// 	IHistogram1D* yProj  = histoSvc()->histogramFactory()->projectionY( "Proj/102" , *m_h2D );
// 	IHistogram1D* xSlice = histoSvc()->histogramFactory()->sliceX( "Slice/101" , *m_h2D, 10);
// 	IHistogram1D* ySlice = histoSvc()->histogramFactory()->sliceY( "Slice/102" , *m_h2D, 20);
// 	log << MSG::DEBUG << "Projection X" << endreq;
//   histoSvc()->print( xProj );
// 	log << MSG::DEBUG << "Projection Y" << endreq;
//   histoSvc()->print( yProj );
// 	// Print the slices
// 	log << MSG::DEBUG << "Slice X:" << endreq;
//   histoSvc()->print( xSlice );
// 	log << MSG::DEBUG << "Slice Y:" << endreq;
//   histoSvc()->print( ySlice );

	 
  return StatusCode::SUCCESS;
}
