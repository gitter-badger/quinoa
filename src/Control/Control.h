//******************************************************************************
/*!
  \file      src/Control/Control.h
  \author    J. Bakosi
  \date      Mon 18 Feb 2013 02:22:58 PM MST
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     Main control category
  \details   Main control catgeory
*/
//******************************************************************************
#ifndef Control_h
#define Control_h

#include <string>

#include <QuinoaTypes.h>
#include <ControlTypes.h>
#include <BackAssociate.h>
#include <Defaults.h>

namespace Quinoa {

using namespace control;

//! Control base
class Control {

  private:
    // Data parsed
    Bundle m_data;

  public:
    //! Constructor
    Control();

    //! Destructor
    ~Control() = default;

    // Set all data in one step by deep-move of whole tuple
    void set(const Bundle& stack) { m_data = move(stack); }

    //! Get single element at position
    template< BundlePosition at >
    const typename std::tuple_element<at, decltype(m_data)>::type& get() {
      return std::get<at>(m_data);
    }

    // Check if an element is different than its default value
    template< BundlePosition at >
    const bool set() {
      return std::get<at>(m_data) != std::get<at>(Defaults);
    }

    //! Get physics keyword
    const std::string& physicsKeyword() {
      return associate::PhysicsKeyword[ std::get<PHYSICS>(m_data) ];
    }
    //! Get physics name
    const std::string& physicsName() {
      return associate::PhysicsName[ std::get<PHYSICS>(m_data) ];
    }

    //! Get hydrodynamics model keyword
    const std::string& hydroKeyword() {
      return associate::HydroKeyword[ std::get<HYDRO>(m_data) ];
    }
    //! Get hydrodynamics model name
    const std::string& hydroName() {
      return associate::HydroName[ std::get<HYDRO>(m_data) ];
    }

    //! Get material mix model keyword
    const std::string& mixKeyword() {
      return associate::MixKeyword[ std::get<MIX>(m_data) ];
    }
    //! Get material mix model name
    const std::string& mixName() {
      return associate::MixName[ std::get<MIX>(m_data) ];
    }

  private:
    //! Don't permit copy constructor
    Control(const Control&) = delete;
    //! Don't permit copy assigment
    Control& operator=(const Control&) = delete;
    //! Don't permit move constructor
    Control(Control&&) = delete;
    //! Don't permit move assigment
    Control& operator=(Control&&) = delete;
};

} // namespace Quinoa

#endif // Control_h
