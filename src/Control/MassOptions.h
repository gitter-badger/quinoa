//******************************************************************************
/*!
  \file      src/Control/MassOptions.h
  \author    J. Bakosi
  \date      Fri Aug  2 15:40:11 2013
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     Mass model options and associations
  \details   Mass model options and associations
*/
//******************************************************************************
#ifndef MassOptions_h
#define MassOptions_h

#include <map>

#include <Exception.h>
#include <Toggle.h>

namespace Quinoa {

namespace select {

//! Mass model types
enum class MassType : uint8_t { NO_MASS=0,
                                BETA };

//! Class with base templated on the above enum class with associations
class Mass : public Toggle<MassType> {

  public:
    //! Constructor initializing associations
    // ICC: use initializer lists
    Mass() : Toggle<MassType>(names, values) {
      //! Enums -> names
      names[MassType::NO_MASS] = "No mass";
      names[MassType::BETA] = "Beta";
      //! keywords -> Enums
      values["no_mass"] = MassType::NO_MASS;
      values["mass_beta"] = MassType::BETA;
    }

  private:
    //! Don't permit copy constructor
    Mass(const Mass&) = delete;
    //! Don't permit copy assigment
    Mass& operator=(const Mass&) = delete;
    //! Don't permit move constructor
    Mass(Mass&&) = delete;
    //! Don't permit move assigment
    Mass& operator=(Mass&&) = delete;

    std::map<MassType, std::string> names;
    std::map<std::string, MassType> values;
};

} // namespace select

} // namespace Quinoa

#endif // MassOptions_h
