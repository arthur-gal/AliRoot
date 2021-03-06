#if !defined(__CINT__) || defined(__MAKECINT__)
  #include <TStopwatch.h>
  #include <TStyle.h>
  #include <TFile.h>
  #include <TH1F.h>
  #include <TH2F.h>
  #include <TString.h>
  #include <TCanvas.h>
  #include "TChain.h"
  #include "TSystem.h"
  #include "TGrid.h"
  #include "AliRawReader.h"
  #include "AliRawReaderChain.h"
  #include "AliCaloRawStreamV3.h"
  #include "AliLog.h"
  #include "iostream"
  #include "Riostream.h"
using namespace std;

#endif

void Pedestals(const TString rawFile="")
{
  // Read raw data, decode it to samples,
  // calculate pedestals from presamples, 
  // evaluate the signal amplitude as a maximum sample, 
  // and fill histograms with pedestals and amplitudes
  // This script should be compiled to speed up the data processing:
  // .L Pedestals.C++
  //___
  // Yuri Kharlov. 6 September 2007

  TStopwatch stopwatch;
  stopwatch.Start();
  
  AliRawReader *reader=0;
  if (rawFile.EndsWith(".root")) {
    if (rawFile.BeginsWith("alien://")) {
      gSystem->Exec("source /tmp/gclient_env_$UID");
      TGrid::Connect("alien://");
    }
    reader = AliRawReader::Create(rawFile);
  }
  else if (rawFile.EndsWith(".txt")) {
    TGrid::Connect("alien://");
    TChain *rawChain = new TChain("RAW");
    ifstream inList;
    inList.open(rawFile);
    char filename[256];
    while (1) {
      inList >> filename;
      if (!inList.good()) break;
      if (rawFile[0] == '!') continue;
      printf("Add file %s to chain\n",filename);
      rawChain->Add(filename);
    }
    inList.close();
    reader = new AliRawReaderChain(rawChain);
  }
  else {
    AliInfoGeneral("",Form("Unknown raw file type %s",rawFile.Data()));
    return;
  }
  reader->Reset();

  TStopwatch timer;
  timer.Start();

  AliCaloRawStreamV3 *stream = new AliCaloRawStreamV3(reader,"PHOS");

  const char * baseNamePed ="hPed";
  const char * baseTitlePed="Ped in cell (";
  const char * sgain[3]={"LG","HG", "TRU"};

  const Int_t caloFlagMax=3,modMax=5,cellXMax=64,cellZMax=56;
  TH1F *hPed[5][3][64][56];
  for (Int_t mod=0; mod<modMax; mod++) {
    for (Int_t caloFlag=0; caloFlag<caloFlagMax; caloFlag++) {
      for (Int_t cellX=0; cellX<cellXMax; cellX++) {
	for (Int_t cellZ=0; cellZ<cellZMax; cellZ++) {
	  hPed[mod][caloFlag][cellX][cellZ] = 0;
	}
      }
    }
  }
  TH1F *hPedHiMean1m1 = new TH1F("hPedHiMean1m1","Mean pedestals in module 1, high gain" ,100,0.,100.);
  TH1F *hPedHiRMS1m1  = new TH1F("hPedHiRMS1m1" ,"RMS pedestals in module 1, high gain"  ,500,0.,50.);
  TH1F *hPedLoMean1m1 = new TH1F("hPedLoMean1m1","Mean pedestals in module 1, low gain"  ,100,0.,100.);
  TH1F *hPedLoRMS1m1  = new TH1F("hPedLoRMS1m1" ,"RMS pedestals in module 1, low gain"   ,500,0.,50.);
  TH1F *hPedTRUMean1m1 = new TH1F("hPedTRUMean1m1","Mean pedestals in module 1, TRU"     ,1000,0.,1000.);
  TH1F *hPedTRURMS1m1  = new TH1F("hPedTRURMS1m1" ,"RMS pedestals in module 1, TRU"      ,500,0.,50.);

  TH1F *hPedHiMean1m2 = new TH1F("hPedHiMean1m2","Mean pedestals in module 2, high gain" ,100,0.,100.);
  TH1F *hPedHiRMS1m2  = new TH1F("hPedHiRMS1m2" ,"RMS pedestals in module 2, high gain"  ,500,0.,50.);
  TH1F *hPedLoMean1m2 = new TH1F("hPedLoMean1m2","Mean pedestals in module 2, low gain"  ,100,0.,100.);
  TH1F *hPedLoRMS1m2  = new TH1F("hPedLoRMS1m2" ,"RMS pedestals in module 2, low gain"   ,500,0.,50.);
  TH1F *hPedTRUMean1m2 = new TH1F("hPedTRUMean1m2","Mean pedestals in module 2, TRU"     ,1000,0.,1000.);
  TH1F *hPedTRURMS1m2  = new TH1F("hPedTRURMS1m2" ,"RMS pedestals in module 2, TRU"      ,500,0.,50.);

  TH1F *hPedHiMean1m3 = new TH1F("hPedHiMean1m3","Mean pedestals in module 3, high gain" ,100,0.,100.);
  TH1F *hPedHiRMS1m3  = new TH1F("hPedHiRMS1m3" ,"RMS pedestals in module 3, high gain"  ,500,0.,50.);
  TH1F *hPedLoMean1m3 = new TH1F("hPedLoMean1m3","Mean pedestals in module 3, low gain"  ,100,0.,100.);
  TH1F *hPedLoRMS1m3  = new TH1F("hPedLoRMS1m3" ,"RMS pedestals in module 3, low gain"   ,500,0.,50.);
  TH1F *hPedTRUMean1m3 = new TH1F("hPedTRUMean1m3","Mean pedestals in module 3, TRU"     ,1000,0.,1000.);
  TH1F *hPedTRURMS1m3  = new TH1F("hPedTRURMS1m3" ,"RMS pedestals in module 3, TRU"      ,500,0.,50.);

  TH1F *hPedHiMean1m4 = new TH1F("hPedHiMean1m4","Mean pedestals in module 4, high gain" ,100,0.,100.);
  TH1F *hPedHiRMS1m4  = new TH1F("hPedHiRMS1m4" ,"RMS pedestals in module 4, high gain"  ,500,0.,50.);
  TH1F *hPedLoMean1m4 = new TH1F("hPedLoMean1m4","Mean pedestals in module 4, low gain"  ,100,0.,100.);
  TH1F *hPedLoRMS1m4  = new TH1F("hPedLoRMS1m4" ,"RMS pedestals in module 4, low gain"   ,500,0.,50.);
  TH1F *hPedTRUMean1m4 = new TH1F("hPedTRUMean1m4","Mean pedestals in module 4, TRU"     ,1000,0.,1000.);
  TH1F *hPedTRURMS1m4  = new TH1F("hPedTRURMS1m4" ,"RMS pedestals in module 4, TRU"      ,500,0.,50.);

  hPedHiMean1m1->Sumw2();
  hPedHiRMS1m1 ->Sumw2();
  hPedLoMean1m1->Sumw2();
  hPedLoRMS1m1 ->Sumw2();
  hPedTRUMean1m1->Sumw2();
  hPedTRURMS1m1 ->Sumw2();
  hPedHiMean1m2->Sumw2();
  hPedHiRMS1m2 ->Sumw2();
  hPedLoMean1m2->Sumw2();
  hPedLoRMS1m2 ->Sumw2();
  hPedTRUMean1m2->Sumw2();
  hPedTRURMS1m2 ->Sumw2();
  hPedHiMean1m3->Sumw2();
  hPedHiRMS1m3 ->Sumw2();
  hPedLoMean1m3->Sumw2();
  hPedLoRMS1m3 ->Sumw2();
  hPedTRUMean1m3->Sumw2();
  hPedTRURMS1m3 ->Sumw2();
  hPedHiMean1m4->Sumw2();
  hPedHiRMS1m4 ->Sumw2();
  hPedLoMean1m4->Sumw2();
  hPedLoRMS1m4 ->Sumw2();
  hPedTRUMean1m4->Sumw2();
  hPedTRURMS1m4 ->Sumw2();

  TH2F *hPedHiMeanm1  = new TH2F("hPedHiMeanm1","Mean pedestals in module 1, high gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);
  TH2F *hPedHiRMSm1   = new TH2F("hPedHiRMSm1" ,"R.M.S. of pedestals in module 1, high gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);
  TH2F *hPedHiNumm1   = new TH2F("hPedHiNumm1" ,"Number of pedestals in module 1, high gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);
  TH2F *hPedLoMeanm1  = new TH2F("hPedLoMeanm1","Mean pedestals in module 1, low gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);
  TH2F *hPedLoRMSm1   = new TH2F("hPedLoRMSm1" ,"R.M.S. of pedestals in module 1, low gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);
  TH2F *hPedLoNumm1   = new TH2F("hPedLoNumm1" ,"Number of pedestals in module 1, low gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);

  TH2F *hPedHiMeanm2  = new TH2F("hPedHiMeanm2","Mean pedestals in module 2, high gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);
  TH2F *hPedHiRMSm2   = new TH2F("hPedHiRMSm2" ,"R.M.S. of pedestals in module 2, high gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);
  TH2F *hPedHiNumm2   = new TH2F("hPedHiNumm2" ,"Number of pedestals in module 2, high gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);
  TH2F *hPedLoMeanm2  = new TH2F("hPedLoMeanm2","Mean pedestals in module 2, low gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);
  TH2F *hPedLoRMSm2   = new TH2F("hPedLoRMSm2" ,"R.M.S. of pedestals in module 2, low gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);
  TH2F *hPedLoNumm2   = new TH2F("hPedLoNumm2" ,"Number of pedestals in module 2, low gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);

  TH2F *hPedHiMeanm3  = new TH2F("hPedHiMeanm3","Mean pedestals in module 3, high gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);
  TH2F *hPedHiRMSm3   = new TH2F("hPedHiRMSm3" ,"R.M.S. of pedestals in module 3, high gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);
  TH2F *hPedHiNumm3   = new TH2F("hPedHiNumm3" ,"Number of pedestals in module 3, high gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);
  TH2F *hPedLoMeanm3  = new TH2F("hPedLoMeanm3","Mean pedestals in module 3, low gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);
  TH2F *hPedLoRMSm3   = new TH2F("hPedLoRMSm3" ,"R.M.S. of pedestals in module 3, low gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);
  TH2F *hPedLoNumm3   = new TH2F("hPedLoNumm3" ,"Number of pedestals in module 3, low gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);

  TH2F *hPedHiMeanm4  = new TH2F("hPedHiMeanm4","Mean pedestals in module 4, high gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);
  TH2F *hPedHiRMSm4   = new TH2F("hPedHiRMSm4" ,"R.M.S. of pedestals in module 4, high gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);
  TH2F *hPedHiNumm4   = new TH2F("hPedHiNumm4" ,"Number of pedestals in module 4, high gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);
  TH2F *hPedLoMeanm4  = new TH2F("hPedLoMeanm4","Mean pedestals in module 4, low gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);
  TH2F *hPedLoRMSm4   = new TH2F("hPedLoRMSm4" ,"R.M.S. of pedestals in module 4, low gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);
  TH2F *hPedLoNumm4   = new TH2F("hPedLoNumm4" ,"Number of pedestals in module 4, low gain",
			       cellXMax,0.,cellXMax, cellZMax,0.,cellZMax);

  TH1I *hNBunches = new TH1I("hNBunches","Number of bunches",10,0,10);
  TH2I *hHWaddr   = new TH2I("hHWaddr","DDL is vs HW addr",216,0,216,4096,0,4096);
  TH1I *hModule   = new TH1I("hModule" ,"Module number", 5,0.,5);

  Int_t iEvent=0;
  Int_t runNum=0;
  Int_t module,cellX,cellZ,caloFlag;

  while (reader->NextEvent()) {
    runNum = reader->GetRunNumber();
    AliInfoGeneral("",Form("Reading event %d\n",iEvent++));
    while (stream->NextDDL()) {
      while (stream->NextChannel()) {
	module   = stream->GetModule();
	cellX    = stream->GetCellX();
	cellZ    = stream->GetCellZ();
	caloFlag = stream->GetCaloFlag();
// 	if (caloFlag!=0 && caloFlag!=1) continue;

  if(caloFlag==2){
    cout << "module = " << module << " , cellX = " << cellX << " , cellZ = " << cellZ << endl;
  }

	//cout << "DDL number = " << stream->GetDDLNumber() << endl;
	//cout << "module = " << module << endl;
	//if(cellX==63 || cellX==0) cout << "cellX = " << cellX << endl;
	//if(cellZ==55 || cellZ==0) cout << "cellZ = " << cellZ << endl;

	hHWaddr->Fill(stream->GetDDLNumber(),stream->GetHWAddress());
	hModule->Fill(module);
	if (!hPed[module][caloFlag][cellX][cellZ]) {
	  TString name;
	  TString title;
	  name  = baseNamePed;
	  title = baseTitlePed;
	  name +="_g"; name +=caloFlag;
	  name +="_m"; name +=module;
	  name +="_x"; name +=cellX;
	  name +="_z"; name +=cellZ;

	  title +=module; title +=",";
	  title +=cellX; title +=",";
	  title +=cellZ; title +="), ";
	  title +=sgain[caloFlag];

	  Int_t nx,xmin,xmax;
	  if (caloFlag==0 || caloFlag==1) {
	    nx=100;
	    xmin=0.;
	    xmax=100.;
	  }
	  else {
	    nx=1000;
	    xmin=0.;
	    xmax=1000.;
	  }
	  hPed[module][caloFlag][cellX][cellZ] = new TH1F(name,title,nx,xmin,xmax);
	  hPed[module][caloFlag][cellX][cellZ]->Sumw2();
	  hPed[module][caloFlag][cellX][cellZ]->SetMarkerStyle(20);
	  hPed[module][caloFlag][cellX][cellZ]->SetOption("eph");
	}

	Int_t nBunches = 0;
	while (stream->NextBunch()) {
	  nBunches++;
	  const UShort_t *sig = stream->GetSignals();
	  Int_t sigLength = stream->GetBunchLength();

// 	  for (Int_t i = sigLength-70; i < sigLength; i++) {
	  for (Int_t i = 0; i < sigLength; i++) {
	    hPed[module][caloFlag][cellX][cellZ]->Fill(sig[i]);
	  }
	}
	hNBunches->Fill(nBunches);
      } // end of NextChannel()

    } // end of NextDDL()
  } // end of nextEvent()

  // Fill 2-dim histograms for mean, rms and n pedestals

  for (Int_t mod=1; mod<=4; mod++) {
    for (Int_t cFlag=0; cFlag<3; cFlag++) {
      for (cellX=0; cellX<cellXMax; cellX++) {
	for (cellZ=0; cellZ<cellZMax; cellZ++) {
	  if (hPed[mod][cFlag][cellX][cellZ] != 0) {
	    if      (cFlag == 0) {
	      if (mod==1) {
		hPedLoMean1m1->Fill( hPed[mod][cFlag][cellX][cellZ]->GetMean());
		hPedLoRMS1m1 ->Fill( hPed[mod][cFlag][cellX][cellZ]->GetRMS() );
		hPedLoMeanm1 ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetMean()    );
		hPedLoRMSm1  ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetRMS()     );
		hPedLoNumm1  ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetEntries() );
	      }
	      else if (mod==2) {
		hPedLoMean1m2->Fill( hPed[mod][cFlag][cellX][cellZ]->GetMean());
		hPedLoRMS1m2 ->Fill( hPed[mod][cFlag][cellX][cellZ]->GetRMS() );
		hPedLoMeanm2 ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetMean()    );
		hPedLoRMSm2  ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetRMS()     );
		hPedLoNumm2  ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetEntries() );
	      }
	      else if (mod==3) {
		hPedLoMean1m3->Fill( hPed[mod][cFlag][cellX][cellZ]->GetMean());
		hPedLoRMS1m3 ->Fill( hPed[mod][cFlag][cellX][cellZ]->GetRMS() );
		hPedLoMeanm3 ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetMean()    );
		hPedLoRMSm3  ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetRMS()     );
		hPedLoNumm3  ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetEntries() );
	      }
	      else if (mod==4) {
		hPedLoMean1m4->Fill( hPed[mod][cFlag][cellX][cellZ]->GetMean());
		hPedLoRMS1m4 ->Fill( hPed[mod][cFlag][cellX][cellZ]->GetRMS() );
		hPedLoMeanm4 ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetMean()    );
		hPedLoRMSm4  ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetRMS()     );
		hPedLoNumm4  ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetEntries() );
	      }
	    }
	    else if (cFlag == 1) {
	      if (mod==1) {
		hPedHiMean1m1->Fill( hPed[mod][cFlag][cellX][cellZ]->GetMean());
		hPedHiRMS1m1 ->Fill( hPed[mod][cFlag][cellX][cellZ]->GetRMS() );
		hPedHiMeanm1 ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetMean()    );
		hPedHiRMSm1  ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetRMS()     );
		hPedHiNumm1  ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetEntries() );
	      }
	      else if (mod==2) {
		hPedHiMean1m2->Fill( hPed[mod][cFlag][cellX][cellZ]->GetMean());
		hPedHiRMS1m2 ->Fill( hPed[mod][cFlag][cellX][cellZ]->GetRMS() );
		hPedHiMeanm2 ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetMean()    );
		hPedHiRMSm2  ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetRMS()     );
		hPedHiNumm2  ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetEntries() );
	      }
	      if (mod==3) {
		hPedHiMean1m3->Fill( hPed[mod][cFlag][cellX][cellZ]->GetMean());
		hPedHiRMS1m3 ->Fill( hPed[mod][cFlag][cellX][cellZ]->GetRMS() );
		hPedHiMeanm3 ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetMean()    );
		hPedHiRMSm3  ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetRMS()     );
		hPedHiNumm3  ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetEntries() );
	      }
	      if (mod==4) {
		hPedHiMean1m4->Fill( hPed[mod][cFlag][cellX][cellZ]->GetMean());
		hPedHiRMS1m4 ->Fill( hPed[mod][cFlag][cellX][cellZ]->GetRMS() );
		hPedHiMeanm4 ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetMean()    );
		hPedHiRMSm4  ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetRMS()     );
		hPedHiNumm4  ->Fill( cellX, cellZ, hPed[mod][cFlag][cellX][cellZ]->GetEntries() );
	      }
	    }
	    else if (cFlag == 2) {
	      if (mod==2) {
		hPedTRUMean1m2->Fill( hPed[mod][cFlag][cellX][cellZ]->GetMean());
		hPedTRURMS1m2 ->Fill( hPed[mod][cFlag][cellX][cellZ]->GetRMS() );
	      }
	      if (mod==3) {
		hPedTRUMean1m3->Fill( hPed[mod][cFlag][cellX][cellZ]->GetMean());
		hPedTRURMS1m3 ->Fill( hPed[mod][cFlag][cellX][cellZ]->GetRMS() );
	      }
	      if (mod==4) {
		hPedTRUMean1m4->Fill( hPed[mod][cFlag][cellX][cellZ]->GetMean());
		hPedTRURMS1m4 ->Fill( hPed[mod][cFlag][cellX][cellZ]->GetRMS() );
	      }
	    }
	  }
	}
      }
    }
  }

  // Write existing histograms to a root file

  TString fileName;
  fileName = "ped";
  fileName += runNum;
  fileName += ".root";
  TFile *file = new TFile(fileName,"RECREATE");

  for (Int_t mod=1; mod<modMax; mod++) {
    for (Int_t cFlag=0; cFlag<caloFlagMax; cFlag++) {
      for (cellX=0; cellX<cellXMax; cellX++) {
	for (cellZ=0; cellZ<cellZMax; cellZ++) {
	  if (hPed[mod][cFlag][cellX][cellZ] != 0)
	    hPed[mod][cFlag][cellX][cellZ]->Write();
	}
      }
    }
  }

  hPedHiMean1m1->Write();
  hPedHiRMS1m1 ->Write();
  hPedLoMean1m1->Write();
  hPedLoRMS1m1 ->Write();
  hPedHiMeanm1 ->Write();
  hPedHiRMSm1  ->Write();
  hPedHiNumm1  ->Write();
  hPedLoMeanm1 ->Write();
  hPedLoRMSm1  ->Write();
  hPedLoNumm1  ->Write();
  hPedTRUMean1m1->Write();
  hPedTRURMS1m1 ->Write();
  
  hPedHiMean1m2->Write();
  hPedHiRMS1m2 ->Write();
  hPedLoMean1m2->Write();
  hPedLoRMS1m2 ->Write();
  hPedHiMeanm2 ->Write();
  hPedHiRMSm2  ->Write();
  hPedHiNumm2  ->Write();
  hPedLoMeanm2 ->Write();
  hPedLoRMSm2  ->Write();
  hPedLoNumm2  ->Write();
  hPedTRUMean1m2->Write();
  hPedTRURMS1m2 ->Write();
  
  hPedHiMean1m3->Write();
  hPedHiRMS1m3 ->Write();
  hPedLoMean1m3->Write();
  hPedLoRMS1m3 ->Write();
  hPedHiMeanm3 ->Write();
  hPedHiRMSm3  ->Write();
  hPedHiNumm3  ->Write();
  hPedLoMeanm3 ->Write();
  hPedLoRMSm3  ->Write();
  hPedLoNumm3  ->Write();
  hPedTRUMean1m3->Write();
  hPedTRURMS1m3 ->Write();

  hPedHiMean1m4->Write();
  hPedHiRMS1m4 ->Write();
  hPedLoMean1m4->Write();
  hPedLoRMS1m4 ->Write();
  hPedHiMeanm4 ->Write();
  hPedHiRMSm4  ->Write();
  hPedHiNumm4  ->Write();
  hPedLoMeanm4 ->Write();
  hPedLoRMSm4  ->Write();
  hPedLoNumm4  ->Write();
  hPedTRUMean1m4->Write();
  hPedTRURMS1m4 ->Write();

  hNBunches  ->Write();
  hHWaddr    ->Write();
  hModule    ->Write();
  
  file->Close();
  stopwatch.Print();
}
