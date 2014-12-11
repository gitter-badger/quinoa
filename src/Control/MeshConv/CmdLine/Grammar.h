//******************************************************************************
/*!
  \file      src/Control/MeshConv/CmdLine/Grammar.h
  \author    J. Bakosi
  \date      Tue 09 Dec 2014 09:14:49 AM MST
  \copyright 2012-2014, Jozsef Bakosi.
  \brief     MeshConv's command line grammar definition
  \details   Grammar definition for parsing the command line. We use the Parsing
  Expression Grammar Template Library (PEGTL) to create the grammar and the
  associated parser. Credit goes to Colin Hirsch (pegtl@cohi.at) for PEGTL. Word
  of advice: read from the bottom up.
*/
//******************************************************************************
#ifndef MeshConvCmdLineGrammar_h
#define MeshConvCmdLineGrammar_h

#include <Grammar.h>
#include <PEGTLParsed.h>
#include <Keywords.h>

namespace meshconv {
//! Mesh converter command line grammar definition
namespace cmd {

  //! PEGTLParsed type specialized to MeshConv's command line parser
  using PEGTLCmdLine =
    tk::ctr::PEGTLParsed< ctr::CmdLine, pegtl::string_input< ctr::Location > >;

  // MeshConv's CmdLine state

  //! Everything is stored in Stack during parsing
  using Stack = PEGTLCmdLine;

  // MeshConv's CmdLine actions

  // MeshConv's CmdLine grammar

  //! verbose (i.e., verbose or quiet output)
  struct verbose :
         tk::grm::process_cmd_switch< Stack,
                                      kw::verbose,
                                      tag::verbose > {};

  //! io parameter
  template< typename keyword, typename io_tag >
  struct io :
         tk::grm::process_cmd< Stack,
                               keyword,
                               tk::grm::Store< Stack, tag::io, io_tag > > {};

  //! command line keywords
  struct keywords :
         pegtl::sor< verbose,
                     io< kw::input, tag::input >,
                     io< kw::output, tag::output > > {};

  //! entry point: parse keywords and until end of string
  struct read_string :
         tk::grm::read_string< Stack, keywords > {};

} // cmd::
} // meshconv::

#endif // MeshConvCmdLineGrammar_h
