#ifndef ALITPCMONITORALTRO_H
#define ALITPCMONITORALTRO_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

////////////////////////////////////////////////////////////////////////
//
// AliTPCMonitorAltro class
//
// Class for decoding raw TPC data in the ALTRO format
// 
// Authors: Roland Bramm, 
//          Stefan Kniege, IKF, Frankfurt
//       
/////////////////////////////////////////////////////////////////////////



#include <iostream>
#include <iomanip>
#include <fstream>
#include "TNamed.h"
#include "AliLog.h"

using namespace std;

class AliTPCMonitorAltro : public TNamed {
 public:
    AliTPCMonitorAltro(UInt_t* memory, Int_t size, Int_t fformat);
    ~AliTPCMonitorAltro();
    
    void         Allocate40BitArray(); 
    void         Allocate10BitArray();
    
    void         Decodeto40Bit();
    void         Decodeto10Bit(Int_t equipment = -1); 

    Int_t        DecodeTrailer(Int_t pos);

    Long64_t*    Get40BitArray();
    Short_t     *Get10BitArray();
    Int_t        Get40BitArraySize(); 
    Int_t        Get10BitArraySize();  
    Char_t*      GetActFileName()        { return ffilename;}
    
    static Int_t GetHwMaskFEC()          { return kHwMaskFEC;}
    static Int_t GetHwMaskBranch()       { return kHwMaskBranch;}
    static Int_t GetHwMaskFECChannel()   { return kHwMaskFECChannel;}
    static Int_t GetHwMaskAltroChannel() { return kHwMaskAltroChannel;}
    static Int_t GetHwMaskAltroChip()    { return kHwMaskAltroChip;}

    static Int_t GetHwMaskRCU()          { return kHwMaskRCU;}
    
    Int_t        GetNextTrailerPos()     { return fNextPos;}

    Int_t        GetTrailerNWords()      { return fTrailerNWords   ;}
    Int_t        GetTrailerHwAddress()   { return fTrailerHwAddress;}
    Int_t        GetTrailerDataPos()     { return fTrailerDataPos  ;}
    Int_t        GetTrailerBlockPos()    { return fTrailerBlockPos ;}
    Int_t        GetTrailerPos()         { return fTrailerPos      ;} 

    void         SetDataOffset(Int_t val){ foffset     =val ;} 
    void         SetWrite10Bit(Int_t wr) { fwrite10bit =wr  ;}
    
  
    void         SetActFilename(Char_t* name){ sprintf(ffilename,"%s",name); } 
    void         SetVerbose(Int_t val)   { fverb=val;}
     
 private:
    
    Int_t                    fverb;                                                     // verbose flag           
    UInt_t*                  fmemory;                                                   // memory pointer fo payload               
    Int_t                    fsize;                                                     // size of fmemory 
    Long64_t*                f40BitArray;                                               // array to store 40 bit words 
    Short_t*                 f10BitArray;                                               // array to store 10 bit words   
    Int_t                    fdecoderPos;                                               // start position for decoding 40 bit words
         
    Bool_t                   fallocate40BitArray;                                       // flag for decoding to 40 bit words
    Bool_t                   fallocate10BitArray;                                       // flag for decoding to 10 bit words
    Int_t                    foffset ;                                                   // data offset (CDH length) 
    Int_t                    fwrite10bit;                                               // flag for writing 10 bit words to file 
    
    Int_t                    fTrailerNWords ;                                           // from Trailer: number of 40 bit words for channel  
    Int_t                    fTrailerHwAddress;                                         // from Trailer: hardware address for current channel
    Int_t                    fTrailerDataPos;                                           // from Trailer: position of first adc value 
    Int_t                    fTrailerBlockPos;                                          // from Trailer: number of 40 bit words for channel
    Int_t                    fTrailerPos;                                               // trailer position
    
    Int_t                    fNextPos;                                                  // position of next trailer
    Char_t*                  ffilename;                                                 // name of processed file
    
    static const Int_t       k24BitOn                = 16777215;                        // bit masks for first 24 bits of 32  for decoding 32 bit words
    static const Int_t       k16BitOn                = 65535;                           // bit masks for first 24 bits of 24
    static const Int_t       k08BitOn                = 255;                             // bit masks for first 24 bits of 8
    
    
    static const Long64_t    kmask10                 = (Long64_t)0x00000000000003FFULL; // mask first   10 bit out of 4o0 bit word 
    static const Long64_t    kmask20                 = (Long64_t)0x00000000000FFC00ULL; // mask second  10 bit out of 4o0 bit word 
    static const Long64_t    kmask30                 = (Long64_t)0x000000003FF00000ULL; // mask third   10 bit out of 4o0 bit word 
    static const Long64_t    kmask40                 = (Long64_t)0x000000FFC0000000ULL; // mask fourth  10 bit out of 4o0 bit word 
    
    static const Long64_t    kTrailerTail            = (Long64_t)0x0000000000002AAAULL; // Tail of the Trailer set to 2AAA 
    static const Long64_t    kTrailerMaskTail        = (Long64_t)0x000000fffC000000ULL; // mask for trailer 
    static const Long64_t    kTrailerMaskHardw       = (Long64_t)0x0000000000000FFFULL; // mask for hardware address
    static const Long64_t    kTrailerMaskNWords      = (Long64_t)0x0000000003FF0000ULL; // mask for nwords  (number of 40 bit data words)
    
    static const Int_t       kHwMaskFEC              = 0x0780;                          // mask for fec in hardware address
    static const Int_t       kHwMaskBranch           = 0x0800;                          // mask for branch in hardware address
    static const Int_t       kHwMaskFECChannel       = 0x007f;                          // mask for fec channel  in hardware address
    static const Int_t       kHwMaskAltroChannel     = 0x000f;                          // mask for altro channel in hardware address
    static const Int_t       kHwMaskAltroChip        = 0x0070;                          // mask for altro chip  in hardware address
    static const Int_t       kHwMaskRCU              = 0x7000;                          // not part of the trailer added afterwards
    
    ClassDef(AliTPCMonitorAltro,1);
};
#endif
