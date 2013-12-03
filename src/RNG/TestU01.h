//******************************************************************************
/*!
  \file      src/RNG/TestU01.h
  \author    J. Bakosi
  \date      Mon 02 Dec 2013 05:43:19 PM MST
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     TestU01 statistical tests
  \details   TestU01 statistical tests
*/
//******************************************************************************
#ifndef TestU01_h
#define TestU01_h

#include <StatTest.h>

namespace rngtest {

//! TestU01 : StatTest
template< typename Result,                                //!< Results type
          Result* (*Creator)(void),                       //!< Results creator
          void (*Deleter)(Result *),                      //!< Results deleter
          StatTest::Pvals (*Run)(unif01_Gen*, Result*) >  //!< Test runner
class TestU01 : public StatTest {

  public:
    //! Constructor
    explicit TestU01( const unif01_Gen* const gen,
                      const quinoa::ctr::RNGType& rng,
                      Names&& names ) :
      m_gen( gen ),
      m_rng( rng ),
      m_npval( names.size() ),
      m_names( std::move(names) ),
      m_res( ResultPtr( Creator() ) ) {};

    //! Destructor
    ~TestU01() noexcept override = default;

    //! Run, pretty awful that TestU01 does not guarantee the constness of gen
    StatTest::Pvals run() override {
      return Run( const_cast<unif01_Gen*>(m_gen), m_res.get() );
    }

    //! Test name accessor
    const Names::value_type&
    name( const Names::size_type& i ) const override {
      Assert( i < m_names.size(), tk::ExceptType::FATAL,
              "Indexing outside of container bounds in TestU01::names()" );
      return m_names[i];
    }

    //! Number of results/test (i.e., p-values) accessor
    const Names::size_type& nresult() const override { return m_npval; }

    //! RNG enum accessor
    const quinoa::ctr::RNGType& rng() const override { return m_rng; }

  private:
    //! Don't permit copy constructor
    TestU01(const TestU01&) = delete;
    //! Don't permit copy assigment
    TestU01& operator=(const TestU01&) = delete;
    //! Don't permit move constructor
    TestU01(TestU01&&) = delete;
    //! Don't permit move assigment
    TestU01& operator=(TestU01&&) = delete;

    const unif01_Gen* const m_gen;             //!< Raw ptr to TestU01 generator
    const quinoa::ctr::RNGType m_rng;          //!< RNG selected
    const Names::size_type m_npval;            //!< Number of p-values produced
    const Names m_names;                       //!< Name(s) of tests

    //! TestU01 results type with a custom deleter by TestU01
    using ResultPtr = TestU01Ptr< Result, Deleter >;
    //! TestU01 results struct (wrapped to std::unique_ptr)
    ResultPtr m_res;
};

} // rngtest::

#endif // TestU01_h