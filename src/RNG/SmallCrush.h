//******************************************************************************
/*!
  \file      src/RNG/SmallCrush.h
  \author    J. Bakosi
  \date      Mon 02 Dec 2013 05:37:41 PM MST
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     SmallCrush battery
  \details   SmallCrush battery
*/
//******************************************************************************
#ifndef SmallCrush_h
#define SmallCrush_h

extern "C" {
  #include <unif01.h>
}

#include <TestU01Util.h>
#include <TestU01Suite.h>

namespace rngtest {

//! SmallCrush : TestU01Suite
class SmallCrush : public TestU01Suite {

  public:
    //! Constructor
    explicit SmallCrush(const Base& base);

    //! Destructor
    ~SmallCrush() noexcept override = default;

    //! Run battery of RNG tests
    void run() override;

    //! Print list of registered statistical tests
    void print() const override;

  private:
    //! Don't permit copy constructor
    SmallCrush(const SmallCrush&) = delete;
    //! Don't permit copy assigment
    SmallCrush& operator=(const SmallCrush&) = delete;
    //! Don't permit move constructor
    SmallCrush(SmallCrush&&) = delete;
    //! Don't permit move assigment
    SmallCrush& operator=(SmallCrush&&) = delete;

    //! Add statistical tests to battery
    void addTests( const Gen01Ptr& gen, const quinoa::ctr::RNGType& rng );

    TestContainer m_tests;                      //!< Statistical tests
    std::vector< StatTest::Pvals > m_pvals;     //!< p-values of tests
};

} // rngtest::

#endif // SmallCrush_h
