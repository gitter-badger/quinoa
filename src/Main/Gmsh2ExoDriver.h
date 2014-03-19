//******************************************************************************
/*!
  \file      src/Main/Gmsh2ExoDriver.h
  \author    J. Bakosi
  \date      Wed Mar 19 08:22:06 2014
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     Gmsh2ExoDriver that drives Gmsh2Exo
  \details   Gmsh2ExoDriver that drives Gmsh2Exo
*/
//******************************************************************************
#ifndef Gmsh2ExoDriver_h
#define Gmsh2ExoDriver_h

#include <Driver.h>
#include <Base.h>

namespace gmsh2exo {

//! Gmsh2ExoDriver : Driver
class Gmsh2ExoDriver : public tk::Driver {

  public:
    //! Constructor
    explicit Gmsh2ExoDriver(int argc, char** argv, const tk::Print& print);

    //! Destructor
    ~Gmsh2ExoDriver() noexcept override = default;

    //! Execute
    void execute() const override;

  private:
    //! Don't permit copy constructor
    Gmsh2ExoDriver(const Gmsh2ExoDriver&) = delete;
    //! Don't permit assigment constructor
    Gmsh2ExoDriver& operator=(const Gmsh2ExoDriver&) = delete;
    //! Don't permit move constructor
    Gmsh2ExoDriver(Gmsh2ExoDriver&&) = delete;
    //! Don't permit move assignment
    Gmsh2ExoDriver& operator=(Gmsh2ExoDriver&&) = delete;
};

} // gmsh2exo::

#endif // Gmsh2ExoDriver_h
