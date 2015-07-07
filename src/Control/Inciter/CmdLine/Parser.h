//******************************************************************************
/*!
  \file      src/Control/Inciter/CmdLine/Parser.h
  \author    J. Bakosi
  \date      Fri 29 May 2015 11:36:28 PM MDT
  \copyright 2012-2015, Jozsef Bakosi.
  \brief     Inciter's command line parser
  \details   This file declares the command-line argument parser for the
     computational shock hydrodynamics tool, Inciter.
*/
//******************************************************************************
#ifndef InciterCmdLineParser_h
#define InciterCmdLineParser_h

#include "StringParser.h"
#include "Inciter/CmdLine/CmdLine.h"

namespace tk { class Print; }

namespace inciter {

//! \brief Command-line parser for Inciter.
//! \details This class is used to interface with PEGTL, for the purpose of
//!   parsing command-line arguments for the computational shock hydrodynamics
//!   tool, Inciter.
//! \author J. Bakosi
class CmdLineParser : public tk::StringParser {

  public:
    //! Constructor
    explicit CmdLineParser( int argc,
                            char** argv,
                            const tk::Print& print,
                            ctr::CmdLine& cmdline,
                            int peid );
};

} // inciter::

#endif // InciterCmdLineParser_h