// $Id$
// $MpId: testExistingPads.C,v 1.12 2005/10/28 15:36:07 ivana Exp $
//
// Test macro for testing which pad is seen as "existing" by 
// by AliMpSectorSegmentation and AliMpFastSegmentation
// To run macro:
// root [0] .L testExistingPads.C+
// root [1] testExistingPads();

#if !defined(__CINT__) || defined(__MAKECINT__)

#include "AliMpStation12Type.h"
#include "AliMpPlaneType.h"
#include "AliMpDataProcessor.h"
#include "AliMpDataMap.h"
#include "AliMpDataStreams.h"
#include "AliMpSector.h"
#include "AliMpSectorReader.h"
#include "AliMpSectorSegmentation.h"
#include "AliMpFastSegmentation.h"
#include "AliMpArea.h"
#include "AliMpVPadIterator.h"
#include "AliMpVPainter.h"

#include <Riostream.h>
#include <TCanvas.h>
#include <TH2.h>

#endif

TCanvas* CreateTCanvas(const TString& name, const TString& title,
                       AliMq::Station12Type station, AliMp::PlaneType plane)
{
  TString newName(name);
  TString newTitle(title);
  TString unique = AliMq::Station12TypeName(station) + AliMp::PlaneTypeName(plane);
  newName += unique;
  newTitle += unique;
  return new TCanvas(newName.Data(), newTitle.Data());
}                     

void testExistingPads(AliMq::Station12Type station,AliMp::PlaneType plane)
{
  AliMpDataProcessor mp;
  AliMpDataMap* dataMap = mp.CreateDataMap("data");
  AliMpDataStreams dataStreams(dataMap);

  AliMpSectorReader r(station, plane);
  AliMpSector* sector = r.BuildSector(dataStreams);
  AliMpSectorSegmentation* segmentation = new AliMpSectorSegmentation(sector);
  AliMpVPainter* painter = AliMpVPainter::CreatePainter(sector);

  TCanvas* c1 = CreateTCanvas("view ",
                            "AliMpSectorPainter::Draw() output (view per pad) ",
                            station, plane);
  painter->Draw("ZSSMP");
  c1->Update();

  Int_t maxPadIndexX = segmentation->MaxPadIndexX();
  Int_t maxPadIndexY = segmentation->MaxPadIndexY();
  
  // Define histogram limits
  Int_t nx = (maxPadIndexX/10 + 1)*10;
  Int_t ny = (maxPadIndexY/10 + 1)*10;
  TH2C* histo = new TH2C("histo","Existing pads", 
                          nx, -0.5, nx-0.5, ny, -0.5, ny-0.5);

  TCanvas* c2 = CreateTCanvas("c2 ","Only existing pads are filled ",
                              station, plane);

  for (Int_t i=0; i<maxPadIndexX+1;i++){
    for (Int_t j=0;j<maxPadIndexY+1;++j){

      if ( segmentation->HasPadByIndices(i,j) ) histo->Fill(i,j);
    }
  }

  c2->cd();
  histo->Draw("box");

  // the same plot with fast segmentation
  TH2C* histo2 = new TH2C("histo2","Existing pads2", 
                          nx, -0.5, nx-0.5, ny, -0.5, ny-0.5);

  TCanvas* c3 = CreateTCanvas("c3 ","Only existing pads are filled ",
                              station, plane);

  AliMpFastSegmentation* fast = new AliMpFastSegmentation(segmentation);
  for (Int_t i=0; i<maxPadIndexX+1;i++){
    for (Int_t j=0;j<maxPadIndexY+1;++j){

      if ( fast->HasPadByIndices(i,j) ) histo2->Fill(i,j);
    }
  }

  c3->cd();
  histo2->Draw("box");
  
  delete fast;
}

void testSt12ExistingPads()
{
  AliMq::Station12Type  station[2] = { AliMq::kStation1, AliMq::kStation2 }; 
  AliMp::PlaneType      plane[2]   = { AliMp::kBendingPlane, AliMp::kNonBendingPlane };
  
  for ( Int_t is = 0; is < 2; is++ ) {
    for ( Int_t ip = 0; ip < 2; ip++ ) {
    
      cout << "Running testExistingPads for " 
           << AliMq::Station12TypeName(station[is]) << "  "
           << AliMp::PlaneTypeName(plane[ip])  << " ... " << endl;
       
      testExistingPads(station[is], plane[ip]);
    
      cout << "... end running " << endl << endl;
    }  
  }   
}  
