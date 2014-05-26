//******************************************************************************
/*!
  \file      src/RNG/RNGSSE.h
  \author    J. Bakosi
  \date      Mon 26 May 2014 07:12:46 AM MDT
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     RNGSSE-based random number generator
  \details   RNGSSE-based random number generator
*/
//******************************************************************************
#ifndef RNGSSE_h
#define RNGSSE_h

#include <make_unique.h>

#include <Exception.h>
#include <Macro.h>

namespace tk {

//! RNGSSE-based random number generator
template< class State, typename SeqNumType, unsigned int (*Generate)(State*) >
class RNGSSE {

    using InitFn = void (*)(State*, SeqNumType);

  public:
    //! Constructor with sequence length option: short, long, medium
    explicit RNGSSE( SeqNumType nthreads,
                     InitFn fnShort,
                     tk::ctr::RNGSSESeqLenType seqlen =
                       tk::ctr::RNGSSESeqLenType::SHORT,
                     InitFn fnLong = nullptr,
                     InitFn fnMed = nullptr) {
      // Select init function based on sequence length specified
      InitFn fn = fnShort;
      if (seqlen == tk::ctr::RNGSSESeqLenType::LONG) fn = fnLong;
      else if (seqlen == tk::ctr::RNGSSESeqLenType::MEDIUM) fn = fnMed;
      // Throw if not NDEBUG and stream init function pointer is invalid
      Assert( fn != nullptr,
              tk::ExceptType::FATAL, "nullptr passed to RNGSSE constructor" );
      // Throw if not NDEBUG and nthreads invalid
      Assert(nthreads > 0, tk::ExceptType::FATAL, "Need at least one thread");
      // Allocate array of stream-pointers for threads
      m_stream = tk::make_unique< State[] >( nthreads );
      // Initialize thread-streams
      for (SeqNumType i=0; i<nthreads; ++i) {
        fn( &m_stream[i], i );
      }
    }

    //! Destructor
    ~RNGSSE() = default;

    //! Uniform RNG
    void uniform( int tid, int num, double* r) const {
      r[0] = static_cast<double>( Generate( &m_stream[tid] ) ) / 4294967296.0;
      IGNORE(num);
    }

    //! Gaussian RNG
    void gaussian(int tid, int num, double* r) const {
      IGNORE(tid);
      IGNORE(num);
      IGNORE(r);
    }

  private:
    //! Don't permit copy constructor
    RNGSSE(const RNGSSE&) = delete;
    //! Don't permit copy assigment
    RNGSSE& operator=(const RNGSSE&) = delete;
    //! Don't permit move constructor
    RNGSSE(RNGSSE&&) = delete;
    //! Don't permit move assigment
    RNGSSE& operator=(RNGSSE&&) = delete;

    //! Random number stream for threads
    std::unique_ptr< State[] > m_stream;
};

} // tk::

#endif // RNGSSE_h
