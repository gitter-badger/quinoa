//******************************************************************************
/*!
  \file      src/Control/BaseKeywords.h
  \author    J. Bakosi
  \date      Thu 31 Oct 2013 08:24:02 PM MDT
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     Basic keywords recognized by all parsers
  \details   Basic keywords recognized by all parsers
*/
//******************************************************************************
#ifndef Keywords
#error "BaseKeywords.h should only be included within a *Keywords.h"
#endif

using namespace pegtl::ascii;

// Keyword 'title'
struct title_info {
  static const char* name() { return "Analysis title"; }
  static const char* help() { return
    "The analysis title may be specified in the input file using the 'title' "
    "keyword. The 'title' keyword must be followed by a doubly-quoted string "
    "specifying the analysis title. Example: title \"Example problem\".";
  }
};
using title = keyword<title_info, t,i,t,l,e >;

// Keyword 'end'
struct end_info {
  static const char* name() { return "End of block"; }
  static const char* help() { return
    "The end of a block is given by the 'end' keyword.";
  }
};
using end = keyword<end_info, e,n,d >;
