//-------------------------------------------------
//
/** \class L1MuGMTReadoutRecord
 *
 *  L1 Global Muon Trigger Readout Record
 *
 *  Contains the data that the GMT will send to readout
 *  for one bunch crossing.
*/
//
//   $Date: 2006/05/15 13:51:42 $
//   $Revision: 1.1 $
//
//   Author :
//   H. Sakulin                  HEPHY Vienna
//
//   Migrated to CMSSW:
//   I. Mikulec
//
//--------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------

#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutRecord.h"

//---------------
// C++ Headers --
//---------------

#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
 
//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTExtendedCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuPacking.h"
#include "SimG4Core/Notification/interface/Singleton.h"

//--------------------------------------
//       class L1MuGMTReadoutRecord
//--------------------------------------

//----------------
// Constructors --
//----------------

L1MuGMTReadoutRecord::L1MuGMTReadoutRecord() {
  reset();
};
 
L1MuGMTReadoutRecord::L1MuGMTReadoutRecord(int bxnr) {
  reset();
  m_BxCounter = bxnr;
};
 
//--------------
// Destructor --
//--------------
L1MuGMTReadoutRecord::~L1MuGMTReadoutRecord() {
};

//--------------
// Operations --
//--------------

/// reset the record
void L1MuGMTReadoutRecord::reset() {

  m_BxCounter = 0;

  for (int i=0; i<4; i++) {
    m_BarrelCands[i]=0;
    m_ForwardCands[i]=0;
    m_GMTCands[i]=0;
  }
       
  m_BrlSortRanks = 0;
  m_FwdSortRanks = 0;

  for (int i=0; i<16; i++) {
    m_InputCands[i] = 0;
  }

  // mip/iso bits
  for (int i=0; i<8; i++) 
    m_MIPbits[i] = m_Quietbits[i] = 0;

}

/// get GMT candidates vector
vector<L1MuGMTExtendedCand>  L1MuGMTReadoutRecord::getGMTCands() const {

  vector<L1MuGMTExtendedCand> cands;

  for (int i=0; i<4; i++) 
    if (m_BarrelCands[i] != 0)
      cands.push_back( L1MuGMTExtendedCand(m_BarrelCands[i], getBrlRank(i), (int) m_BxCounter ) );
  
  for (int i=0; i<4; i++) 
    if (m_ForwardCands[i] != 0)
      cands.push_back( L1MuGMTExtendedCand(m_ForwardCands[i], getFwdRank(i), (int) m_BxCounter ) );
    
  // sort by rank
  stable_sort( cands.begin(), cands.end(), L1MuGMTExtendedCand::RankRef() );

  return cands;
}

/// get GMT barrel candidates vector
vector<L1MuGMTExtendedCand> L1MuGMTReadoutRecord::getGMTBrlCands() const {

  vector<L1MuGMTExtendedCand> cands;
  
  for (int i=0; i<4; i++) 
    if (m_BarrelCands[i] != 0)
      cands.push_back( L1MuGMTExtendedCand(m_BarrelCands[i], getBrlRank(i), (int) m_BxCounter ) );
  
  return cands;
}

/// get GMT forward candidates vector
vector<L1MuGMTExtendedCand> L1MuGMTReadoutRecord::getGMTFwdCands() const {

  vector<L1MuGMTExtendedCand> cands;
  
  for (int i=0; i<4; i++) 
    if (m_ForwardCands[i] != 0)
      cands.push_back( L1MuGMTExtendedCand(m_ForwardCands[i], getFwdRank(i), (int) m_BxCounter ) );

  return cands;
}

/// get DT candidates vector
vector<L1MuRegionalCand> L1MuGMTReadoutRecord::getDTBXCands() const {

  vector<L1MuRegionalCand> cands;
  
  for (int i=0; i<4; i++) 
    if (m_InputCands[i] != 0)
      cands.push_back( L1MuRegionalCand(m_InputCands[i], (int) m_BxCounter));
  
  return cands;
}


/// get CSC candidates vector
vector<L1MuRegionalCand> L1MuGMTReadoutRecord::getCSCCands() const {

  vector<L1MuRegionalCand> cands;
  
  for (int i=0; i<4; i++) 
    if (m_InputCands[i+8] != 0)
      cands.push_back( L1MuRegionalCand(m_InputCands[i+8], (int) m_BxCounter));
  
  return cands;
}

/// get barrel RPC candidates vector
vector<L1MuRegionalCand> L1MuGMTReadoutRecord::getBrlRPCCands() const {

  vector<L1MuRegionalCand> cands;
  
  for (int i=0; i<4; i++) 
    if (m_InputCands[i+4] != 0)
      cands.push_back( L1MuRegionalCand(m_InputCands[i+4], (int) m_BxCounter));
  
  return cands;
}

/// get forward RPC candidates vector
vector<L1MuRegionalCand> L1MuGMTReadoutRecord::getFwdRPCCands() const {

  vector<L1MuRegionalCand> cands;
  
  for (int i=0; i<4; i++) 
    if (m_InputCands[i+12] != 0)
      cands.push_back( L1MuRegionalCand(m_InputCands[i+12], (int) m_BxCounter));
  
  return cands;
}



//
// Setters
//

/// set GMT barrel candidate
void L1MuGMTReadoutRecord::setGMTBrlCand(int nr, L1MuGMTExtendedCand const& cand) {
  if (nr>=0 && nr<4) {
    m_BarrelCands[nr] = cand.getDataWord();
    setBrlRank(nr, cand.rank());
  }
};

/// set GMT barrel candidate
void L1MuGMTReadoutRecord::setGMTBrlCand(int nr, unsigned data, unsigned rank) {
  if (nr>=0 && nr<4) {
    m_BarrelCands[nr] = data;
    setBrlRank(nr, rank);
  }
};

/// set GMT forward candidate
void L1MuGMTReadoutRecord::setGMTFwdCand(int nr, L1MuGMTExtendedCand const& cand) {
  if (nr>=0 && nr<4) {
    m_ForwardCands[nr] = cand.getDataWord();
    setFwdRank(nr, cand.rank());
  }
};

/// set GMT forward candidate
void L1MuGMTReadoutRecord::setGMTFwdCand(int nr, unsigned data, unsigned rank) {
  if (nr>=0 && nr<4) {
    m_ForwardCands[nr] = data;
    setFwdRank(nr, rank);
  }
};

/// set GMT candidate
void L1MuGMTReadoutRecord::setGMTCand(int nr, L1MuGMTExtendedCand const& cand) {
  if (nr>=0 && nr<4) {
    m_GMTCands[nr] = cand.getDataWord();
  }
};

/// set GMT candidate
void L1MuGMTReadoutRecord::setGMTCand(int nr, unsigned data) {
  if (nr>=0 && nr<4) {
    m_GMTCands[nr] = data;
  }
};


/// set MIP bit
void L1MuGMTReadoutRecord::setMIPbit(int eta, int phi) {

  if (phi<0 || phi > 17 || eta < 0 || eta > 13) return;

  int idx = eta * 18 + phi;
  int idx_word = idx / 32;
  int idx_bit = idx % 32;

  unsigned mask = 1 << (idx_bit-1);
  
  m_MIPbits[idx_word] |= mask;

};


/// set Quiet bit
void L1MuGMTReadoutRecord::setQuietbit(int eta, int phi) {

  if (phi<0 || phi > 17 || eta < 0 || eta > 13) return;

  int idx = eta * 18 + phi;
  int idx_word = idx / 32;
  int idx_bit = idx % 32;

  unsigned mask = 1 << (idx_bit-1);
  
  m_Quietbits[idx_word] |= mask;

};


 /// get rank of brl cand i
unsigned L1MuGMTReadoutRecord::getBrlRank(int i) const {

  unsigned mask = ( (1 << 8)-1 ) << (i*8);
  return (m_BrlSortRanks & mask) >> (i*8);

};


/// get rank of fwd cand i
unsigned L1MuGMTReadoutRecord::getFwdRank(int i) const {

  unsigned mask = ( (1 << 8)-1 ) << (i*8);
  return (m_FwdSortRanks & mask) >> (i*8);

};

/// set rank of brl cand i
void L1MuGMTReadoutRecord::setBrlRank(int i, unsigned value) {

  unsigned mask = ( (1 << 8)-1 ) << (i*8);
  m_BrlSortRanks &= ~mask;
  m_BrlSortRanks |= value << (i*8);

};


/// set rank of fwd cand i
void L1MuGMTReadoutRecord::setFwdRank(int i, unsigned value) {

  unsigned mask = ( (1 << 8)-1 ) << (i*8);
  m_FwdSortRanks &= ~mask;
  m_FwdSortRanks |= value << (i*8);

};


/// get MIP bit
unsigned L1MuGMTReadoutRecord::getMIPbit(int eta, int phi) const {

  if (phi<0 || phi > 17 || eta < 0 || eta > 13) return 0;

  int idx = eta * 18 + phi;
  int idx_word = idx / 32;
  int idx_bit = idx % 32;

  unsigned mask = 1 << (idx_bit-1);
  
  return( m_MIPbits[idx_word] & mask) ? 1 : 0;

};


/// get Quiet bit
unsigned L1MuGMTReadoutRecord::getQuietbit(int eta, int phi) const {

  if (phi<0 || phi > 17 || eta < 0 || eta > 13) return 0;

  int idx = eta * 18 + phi;
  int idx_word = idx / 32;
  int idx_bit = idx % 32;

  unsigned mask = 1 << (idx_bit-1);
  
  return( m_Quietbits[idx_word] & mask) ? 1 : 0;

};























