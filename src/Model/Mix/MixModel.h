//******************************************************************************
/*!
  \file      src/Model/Mix/MixModel.h
  \author    J. Bakosi
  \date      Fri 09 Nov 2012 05:42:53 AM MST
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     MixModel base
  \details   MixModel base
*/
//******************************************************************************
#ifndef MixModel_h
#define MixModel_h

#include <Model.h>

namespace Quinoa {

//! MixModel base
class MixModel : public Model {

  public:
    //! Constructor
    MixModel() = default;

    //! Destructor
    virtual ~MixModel() {}

    //! Interface for setting initial conditions
    virtual void setIC() = 0;

  private:
    //! Don't permit copy constructor
    MixModel(const MixModel&) = delete;
    //! Don't permit copy assigment
    MixModel& operator=(const MixModel&) = delete;
    //! Don't permit move constructor
    MixModel(MixModel&&) = delete;
    //! Don't permit move assigment
    MixModel& operator=(MixModel&&) = delete;
};

} // namespace Quinoa

#endif // MixModel_h
