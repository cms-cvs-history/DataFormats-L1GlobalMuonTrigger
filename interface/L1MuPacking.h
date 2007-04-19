//-------------------------------------------------
//
/* L1MuPacking
 *
 * define abstract packing and three implementations
 *
*/
//
//   $Date: 2006/08/21 14:26:07 $
//   $Revision: 1.2 $
//
//   Author :
//   H. Sakulin            HEPHY Vienna
//
//   Migrated to CMSSW:
//   I. Mikulec
//
//--------------------------------------------------


#ifndef DataFormatsL1GlobalMuonTrigger_L1MuPacking_h
#define DataFormatsL1GlobalMuonTrigger_L1MuPacking_h

#include "FWCore/MessageLogger/interface/MessageLogger.h"

/**
 * \class L1MuPacking
 *
 *  Abstract Packing of an int in a bit-field
*/

class L1MuPacking {
 public:
  /// get the sign from the packed notation (0=positive, 1=negative)
  virtual int signFromPacked(unsigned packed) const = 0;
  /// get the value from the packed notation
  virtual int idxFromPacked(unsigned packed) const = 0;
  /// get the packed notation of a value 
  virtual unsigned packedFromIdx(int idx) const = 0;
};

/**
 * \class L1MuUnsignedPacking
 *
 * Packing of an unsigned int in a bit field
*/

template<unsigned int Bits> 
class L1MuUnsignedPacking : public L1MuPacking{
 public:
  /// get the sign from the packed notation. always psitive (0)
  virtual int signFromPacked(unsigned packed) const { return 0;}; 
  /// get the value from the packed notation (always positive)
  virtual int idxFromPacked(unsigned packed) const { return (int) packed;};
  /// get the packed notation of a value, check the range
  virtual unsigned packedFromIdx(int idx) const { 
    if (idx >= (1 << Bits) ) edm::LogWarning("ScaleRangeViolation") 
                  << "L1MuUnignedPacking::packedFromIdx: warning value " << idx 
		  << "exceeds " << Bits << "-bit range !!!";        
    return (unsigned) idx;
  };
};

/**
 * \class L1MuSignedPacking
 *
 * Packing of a signed int in a bit field (2's complement)
*/

template<unsigned int Bits>
class L1MuSignedPacking : public L1MuPacking {
 public:
  /// get the sign from the packed notation (0=positive, 1=negative)
  virtual int signFromPacked(unsigned packed) const { return packed & (1 << (Bits-1)) ? 1 : 0;};

  /// get the value from the packed notation (+/-)
  virtual int idxFromPacked(unsigned packed) const { return packed & (1 << (Bits-1)) ? (packed - (1 << Bits) ) : packed;};
  /// get the packed notation of a value, check range
  virtual unsigned packedFromIdx(int idx) const { 
    unsigned maxabs = 1 << (Bits-1) ;
    if (idx < -(int)maxabs && idx >= (int)maxabs) edm::LogWarning("ScaleRangeViolation") 
                                                       << "L1MuSignedPacking::packedFromIdx: warning value " << idx 
						       << "exceeds " << Bits << "-bit range !!!";    
    return  ~(~0 << Bits) & (idx < 0 ? (1 << Bits) + idx : idx);
  };
};

/**
 * \class L1MuPseudoSignedPacking
 *
 * Packing of a signed int in a bit field (pseudo-sign)
 *
 * There is a -0 and a +0 in the pseudo-signed scale
*/

class L1MuPseudoSignedPacking : public L1MuPacking {
 public:
  L1MuPseudoSignedPacking(unsigned int nbits) : m_nbits(nbits) {};

  /// get the (pseudo-)sign from the packed notation (0=positive, 1=negative)
  virtual int signFromPacked(unsigned packed) const { return ( packed & (1 << (m_nbits-1)) ) ? 1 : 0;};

  /// get the value from the packed notation (+/-)
  virtual int idxFromPacked(unsigned packed) const {
    unsigned mask = (1 << (m_nbits-1)) - 1; // for lower bits
    int absidx = (int) ( packed & mask );
    unsigned psmask = (1 << (m_nbits-1) );
    return absidx * ( ( (packed & psmask) == psmask ) ? -1 : 1 ); // pseudo sign==1 is negative
  };  
  /// get the packed notation of a value, check range
  virtual unsigned packedFromIdx(int idx) const {
    unsigned packed = abs(idx);
    unsigned maxabs = (1 << (m_nbits-1)) -1;
    if (packed > maxabs) edm::LogWarning("ScaleRangeViolation") 
                              << "L1MuPseudoSignedPacking::packedFromIdx: warning value " << idx 
			      << "exceeds " << m_nbits << "-bit range !!!";
    if (idx < 0) packed |= 1 << (m_nbits-1);
    return  packed;
  }

  /// get the packed notation of a value, check range; sets the sign separately, 1 is neg. sign(!)
  virtual unsigned packedFromIdx(int idx, int sig) const {
    unsigned packed = abs(idx);
    unsigned maxabs = (1 << (m_nbits-1)) -1;
    if (packed > maxabs) edm::LogWarning("ScaleRangeViolation") 
                              << "L1MuPseudoSignedPacking::packedFromIdx: warning value " << idx 
			      << "exceeds " << m_nbits << "-bit range !!!";
    if (sig==1) packed |= 1 << (m_nbits-1); // sig==1 is negative
    return  packed;
  }

 private:
  unsigned int m_nbits;
};

#endif