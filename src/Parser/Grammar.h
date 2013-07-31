//******************************************************************************
/*!
  \file      src/Parser/Grammar.h
  \author    J. Bakosi
  \date      Tue 30 Jul 2013 08:09:41 PM MDT
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     Grammar definition
  \details   Grammar definition. We use the Parsing Expression Grammar Template
             Library (PEGTL) to create the grammar and the associated parser.
             Credit goes to Colin Hirsch (pegtl@cohi.at) for PEGTL. Word of
             advice: read from the bottom up.
*/
//******************************************************************************
#ifndef Grammar_h
#define Grammar_h

#include <algorithm>

#include <Macro.h>
#include <ControlTypes.h>
#include <Option.h>
#include <Box.h>

namespace Quinoa {

namespace grammar {

  using namespace pegtl;
  using namespace pegtl::ascii;

  // Keywords

  #include <Keywords.h>

  // State

  //! Bundle is where everything is stored during parsing
  using Stack = control::Bundle;
  //! BoolBundle indicates stored fields (true or false)
  using BoolStack = control::BoolBundle;
  //! Out-of-struct storage of field ID for push_term
  static int field = 0;
  //! Geometry definition options
  static control::Option<select::Geometry> Geometry;
  //! Physics options
  static control::Option<select::Physics> Physics;
  //! Position options
  static control::Option<select::Position> Position;
  //! Mass options
  static control::Option<select::Mass> Mass;
  //! Hydro options
  static control::Option<select::Hydro> Hydro;
  //! Mix options
  static control::Option<select::Mix> Mix;
  //! Turbulence frequency options
  static control::Option<select::Frequency> Frequency;
  //! RNG test (suite) options
  static control::Option<select::RNGTest> RNGTest;

  // Actions

  // convert string to 'type'
  template< class type >
  static type convert(const std::string& str) {
    std::stringstream ss(str);
    type num;
    ss >> num;
    return num;
  }

  // convert 'type' to string
  template< class type >
  static std::string convert(const type& val) {
    std::stringstream ss;
    ss << val;
    return ss.str();
  }

  // convert & store value in state 'at' position
  template< control::BundlePosition at >
  struct cstore : action_base< cstore<at> > {
    static void apply(const std::string& value,
                      Stack& stack,
                      BoolStack& boolstack) {
      // Convert to correct type and store element at position 'at'
      std::get<at>(stack) =
        convert< typename std::tuple_element<at,Stack>::type >(value);
      boolstack[at] = true;
    }
  };

  // store value in state 'at' position (no conversion)
  template< control::BundlePosition at >
  struct store : action_base< store<at> > {
    static void apply(const std::string& value,
                      Stack& stack,
                      BoolStack& boolstack) {
      std::get<at>(stack) = value;
      boolstack[at] = true;
    }
  };

  // push value in state 'at' position
  template< control::BundlePosition at >
  struct push : action_base< push<at> > {
    static void apply(const std::string& value,
                      Stack& stack,
                      BoolStack& boolstack) {
      // Convert to correct type and push element to vector at position 'at'
      std::get<at>(stack).push_back(convert<real>(value));
      boolstack[at] = true;
    }
  };

  // start new product in vector of statistics
  struct start_product : action_base< start_product > {
    static void apply(const std::string& value,
                      Stack& stack,
                      BoolStack& boolstack) {
      std::get<control::STATISTICS>(stack).push_back(
        std::vector<control::Term>());
      boolstack[control::STATISTICS] = true;
      IGNORE(value);        // suppress compiler warning on unused variable
    }
  };

  // add matched value as Term into vector of Product in vector of statistics
  template< control::Quantity quantity, control::Moment moment, char name='\0' >
  struct push_term : action_base< push_term<quantity, moment, name> > {
    static void apply(const std::string& value,
                      Stack& stack,
                      BoolStack& boolstack) {

      // if name is given, push name, otherwise push first char of value
      char n(name ? name : value[0]);
      // if name is given, it is triggered not user-requested
      bool plot(name ? false : true);
      // Use stats for shorthand of reference in bundle
      std::vector<control::Product>& stats =
        std::get<control::STATISTICS>(stack);
      // Push term into current product
      stats.back().push_back(control::Term(field, quantity, moment, n, plot));

      // If central moment, trigger mean
      if (moment == control::Moment::CENTRAL) {
        control::Term term(field,
                           quantity,
                           control::Moment::ORDINARY,
                           toupper(n),
                           false);
        stats.insert(stats.end()-1, control::Product(1,term));
      }

      field = 0;            // reset default field
      IGNORE(boolstack);    // suppress compiler warning on unused variable
    }
  };

  // save field ID so push_term can pick it up
  struct save_field : action_base< save_field > {
    static void apply(const std::string& value,
                      Stack& stack,
                      BoolStack& boolstack) {
      field = convert<int>(value) - 1;  // numbering of field IDs start from 0
      IGNORE(stack);        // suppress compiler warning on unused variable
      IGNORE(boolstack);    // suppress compiler warning on unused variable
    }
  };

  // store selected physics
  struct store_physics : action_base< store_physics > {
    static void apply(const std::string& value,
                      Stack& stack,
                      BoolStack& boolstack) {
      // Issue warning if overwrite
      if (boolstack[control::PHYSICS]) {
        std::cout << ">>> PARSER WARNING: Multiple physics defined in input "
                     "file" << std::endl << ">>> Overwriting \""
                  << Physics.name(std::get<control::PHYSICS>(stack))
                  << "\" with \""
                  << Physics.name(Physics.option(value)) << "\""
                  << std::endl;
      }
      std::get<control::PHYSICS>(stack) = Physics.option(value);
      boolstack[control::PHYSICS] = true;
    }
  };

  // store selected position model
  struct store_position : action_base< store_position > {
    static void apply(const std::string& value,
                      Stack& stack,
                      BoolStack& boolstack) {
      // Issue warning if overwrite
      if (boolstack[control::POSITION]) {
        std::cout << ">>> PARSER WARNING: Multiple position models defined in "
                     "input file" << std::endl << ">>> Overwriting \""
                  << Position.name(std::get<control::POSITION>(stack))
                  << "\" with \""
                  << Position.name(Position.option(value))
                  << "\"" << std::endl;
      }
      std::get<control::POSITION>(stack) = Position.option(value);
      boolstack[control::POSITION] = true;
    }
  };

  // store selected mass model
  struct store_mass : action_base< store_mass > {
    static void apply(const std::string& value,
                      Stack& stack,
                      BoolStack& boolstack) {
      // Issue warning if overwrite
      if (boolstack[control::MASS]) {
        std::cout << ">>> PARSER WARNING: Multiple mass models defined in "
                     "input file" << std::endl << ">>> Overwriting \""
                  << Mass.name(std::get<control::MASS>(stack))
                  << "\" with \""
                  << Mass.name(Mass.option(value)) << "\""
                  << std::endl;
      }
      std::get<control::MASS>(stack) = Mass.option(value);
      boolstack[control::MASS] = true;
    }
  };

  // store selected hydrodynamics model
  struct store_hydro : action_base< store_hydro > {
    static void apply(const std::string& value,
                      Stack& stack,
                      BoolStack& boolstack) {
      // Issue warning if overwrite
      if (boolstack[control::HYDRO]) {
        std::cout << ">>> PARSER WARNING: Multiple hydro models defined in "
                     "input file" << std::endl << ">>> Overwriting \""
                  << Hydro.name(std::get<control::HYDRO>(stack))
                  << "\" with \""
                  << Hydro.name(Hydro.option(value)) << "\""
                  << std::endl;
      }
      std::get<control::HYDRO>(stack) = Hydro.option(value);
      boolstack[control::HYDRO] = true;
    }
  };

  // store selected material mix model
  struct store_mix : action_base< store_mix > {
    static void apply(const std::string& value,
                      Stack& stack,
                      BoolStack& boolstack) {
      // Issue warning if overwrite
      if (boolstack[control::MIX]) {
        std::cout << ">>> PARSER WARNING: Multiple mix models defined in input "
                     "file" << std::endl << ">>> Overwriting \""
                  << Mix.name(std::get<control::MIX>(stack))
                  << "\" with \""
                  << Mix.name(Mix.option(value)) << "\"" << std::endl;
      }
      std::get<control::MIX>(stack) = Mix.option(value);
      boolstack[control::MIX] = true;
    }
  };

  // store selected turbulence frequency model
  struct store_freq : action_base< store_freq > {
    static void apply(const std::string& value,
                      Stack& stack,
                      BoolStack& boolstack) {
      // Issue warning if overwrite
      if (boolstack[control::FREQUENCY]) {
        std::cout << ">>> PARSER WARNING: Multiple frequency models defined in "
                     "input file" << std::endl << ">>> Overwriting \""
                  << Frequency.name(std::get<control::FREQUENCY>(stack))
                  << "\" with \""
                  << Frequency.name(Frequency.option(value)) << "\""
                  << std::endl;
      }
      std::get<control::FREQUENCY>(stack) = Frequency.option(value);
      boolstack[control::FREQUENCY] = true;
    }
  };

  // store selected geometry definition
  struct store_geometry : action_base< store_geometry > {
    static void apply(const std::string& value,
                      Stack& stack,
                      BoolStack& boolstack) {
      // Issue warning if overwrite
      if (boolstack[control::GEOMETRY]) {
        std::cout << ">>> PARSER WARNING: Multiple geometry definitions "
                     "defined in input file" << std::endl
                  << ">>> Overwriting \""
                  << Geometry.name(std::get<control::GEOMETRY>(stack))
                  << "\" with \""
                  << Geometry.name(Geometry.option(value)) << "\"" << std::endl;
      }
      std::get<control::GEOMETRY>(stack) = Geometry.option(value);
      boolstack[control::GEOMETRY] = true;
    }
  };

  // store selected RNG test suite
  struct store_rngtest : action_base< store_rngtest > {
    static void apply(const std::string& value,
                      Stack& stack,
                      BoolStack& boolstack) {
      // Issue warning if overwrite
      if (boolstack[control::RNGTEST]) {
        std::cout << ">>> PARSER WARNING: Multiple RNG test suites defined in "
                     "input file" << std::endl << ">>> Overwriting \""
                  << RNGTest.name(std::get<control::RNGTEST>(stack))
                  << "\" with \""
                  << RNGTest.name(RNGTest.option(value)) << "\"" << std::endl;
      }
      std::get<control::RNGTEST>(stack) = RNGTest.option(value);
      boolstack[control::RNGTEST] = true;
    }
  };


  // Grammar

  // read 'token' until 'erased' trimming 'erased'
  template< class token, class erased >
  struct trim :
         seq< token, until< at<erased> > > {};

  // read 'token' padded by blank at left and space at right
  template< class token >
  struct read :
         pad< trim<token, space>, blank, space > {};

  // match all accepted as position models
  struct position : sor< keyword::pos_inviscid,
                         keyword::pos_viscous > {};

  // match all accepted as hydro models
  struct hydro : sor< keyword::hydro_slm,
                      keyword::hydro_glm > {};

  // match all accepted as mix models
  struct mix : sor< keyword::mix_iem,
                    keyword::mix_iecm,
                    keyword::mix_dir,
                    keyword::mix_gendir > {};

  // parse input padded by blank at left and space at right and if it matches
  // 'keywords', apply 'actions'
  template< class keywords, typename ... actions >
  struct parse :
         pad< ifapply< trim<keywords, space>, actions ... >, blank, space > {};

  // comment: start with '#' until eol
  struct comment :
         pad< trim<one<'#'>,eol>, blank, eol> {};

  // plow through block of 'tokens' until 'end' keyword
  template< typename ... tokens >
  struct block :
         until< read<keyword::end>, sor<comment, tokens ...> > {};

  // number: optional sign followed by digits
  struct number :
         seq< opt< sor<one<'+'>, one<'-'>> >, digit> {};

  // plow through list of values between keywords 'key' and "end", calling
  // 'insert' for each if matches and allowing comments between values
  template< class key, class insert, class value = number >
  struct list :
         ifmust< read<key>,
                 until< read<keyword::end>, sor<comment, parse<value,insert>> >
               > {};

  // process 'keyword' and call its 'insert' action if matches 'keywords'
  template< class keyword, class insert, class keywords = alnum >
  struct process :
         ifmust< read<keyword>, parse<keywords,insert> > {};

  // moment: 'keyword' optionally followed by a digit, pushed to vector of terms
  template< class keyword, control::Quantity q, control::Moment m >
  struct moment :
         sor < ifapply<seq<keyword, ifapply<digit,save_field>>, push_term<q,m>>,
               ifapply<keyword, push_term<q,m>>
             > {};

  // terms recognized within an expectation and their mapping
  struct terms :
         sor< moment<keyword::transported_scalar,
                     control::Quantity::SCALAR,
                     control::Moment::ORDINARY>,
              moment<keyword::transported_scalar_fluctuation,
                     control::Quantity::SCALAR,
                     control::Moment::CENTRAL>,
              moment<keyword::velocity_x,
                     control::Quantity::VELOCITY_X,
                     control::Moment::ORDINARY>
            > {};

  // plow through terms in expectation until character 'rbound'
  template< char rbound >
  struct expectation :
         until< one<rbound>, terms > {};

  // plow through expectations between characters 'lbound' and 'rbound'
  template< char lbound, char rbound >
  struct parse_expectations :
         read< ifmust< one<lbound>, apply<start_product>, expectation<rbound> >
             > {};

  // scan problem title between characters 'lbound' and 'rbound'
  template< char lbound, char rbound >
  struct parse_title :
         ifmust< one<lbound>,
                 ifapply< trim<not_one<lbound>, one<rbound>>,
                          store<control::TITLE> >,
                 one<rbound>
               > {};

  // title
  struct title :
         ifmust< read<keyword::title>, parse_title<'"','"'> > {};

  // analytic_geometry block
  struct analytic_geometry:
         ifmust< parse<keyword::analytic_geometry, store_geometry>,
                 block< list<keyword::box, push<control::BOXES>>
                      >
               > {};

  // discrete_geometry block
  struct discrete_geometry:
         ifmust< parse<keyword::discrete_geometry, store_geometry>,
                 block< list<keyword::box, push<control::BOXES>>
                      >
               > {};

  // dir block
  struct dir :
         ifmust< parse<keyword::mix_dir, store_mix>,
                 block< process<keyword::nscalar, cstore<control::NSCALAR>>,
                        list<keyword::dir_B, push<control::B>>,
                        list<keyword::dir_S, push<control::S>>,
                        list<keyword::dir_kappa, push<control::KAPPA>>
                      >
               > {};

  // gendir block
  struct gendir :
         ifmust< parse<keyword::mix_gendir, store_mix>,
                 block< process<keyword::nscalar, cstore<control::NSCALAR>>,
                        list<keyword::dir_B, push<control::B>>,
                        list<keyword::dir_S, push<control::S>>,
                        list<keyword::dir_kappa, push<control::KAPPA>>,
                        list<keyword::gendir_C, push<control::C>>
                      >
               > {};

  // statistics block
  struct statistics :
         ifmust< read< keyword::statistics >,
                 block< parse_expectations<'<','>'> >
               > {};

  // slm block
  struct slm :
         ifmust< parse< keyword::hydro_slm, store_hydro,
                        // trigger estimating the diagonal of Reynolds-stress
                        start_product,
                        push_term<control::Quantity::VELOCITY_X,
                                  control::Moment::CENTRAL, 'u'>,
                        push_term<control::Quantity::VELOCITY_X,
                                  control::Moment::CENTRAL, 'u'>,
                        start_product,
                        push_term<control::Quantity::VELOCITY_Y,
                                  control::Moment::CENTRAL, 'v'>,
                        push_term<control::Quantity::VELOCITY_Y,
                                  control::Moment::CENTRAL, 'v'>,
                        start_product,
                        push_term<control::Quantity::VELOCITY_Z,
                                  control::Moment::CENTRAL, 'w'>,
                        push_term<control::Quantity::VELOCITY_Z,
                                  control::Moment::CENTRAL, 'w'>
                      >,
                 block< process<keyword::SLM_C0, cstore<control::C0>>,
                        process<keyword::nvelocity, cstore<control::NVELOCITY>>
                      >
               > {};

  // freq_gamma block
  struct freq_gamma :
         ifmust< parse< keyword::freq_gamma, store_freq >,
                 block< process<keyword::nfreq, cstore<control::NFREQUENCY>>,
                        process<keyword::freq_gamma_C1,
                                cstore<control::FREQ_GAMMA_C1>>,
                        process<keyword::freq_gamma_C2,
                                cstore<control::FREQ_GAMMA_C2>>,
                        process<keyword::freq_gamma_C3,
                                cstore<control::FREQ_GAMMA_C3>>,
                        process<keyword::freq_gamma_C4,
                                cstore<control::FREQ_GAMMA_C4>> >
               > {};

  // beta block
  struct beta :
         ifmust< parse< keyword::mass_beta, store_mass >,
                 block< process<keyword::ndensity, cstore<control::NDENSITY>>,
                        process<keyword::Beta_At, cstore<control::AT>> >
               > {};

  // geometry definition types
  struct geometry :
         sor< analytic_geometry,
              discrete_geometry > {};

  // common to all physics
  struct physics_common :
         sor< process<keyword::nstep, cstore<control::NSTEP>>,
              process<keyword::term, cstore<control::TERM>>,
              process<keyword::dt, cstore<control::DT>>,
              process<keyword::npar, cstore<control::NPAR>>,
              process<keyword::input, store<control::INPUT>>,
              process<keyword::output, store<control::OUTPUT>>,
              process<keyword::pdfname, store<control::PDFNAME>>,
              process<keyword::globname, store<control::GLOBNAME>>,
              process<keyword::statname, store<control::STATNAME>>,
              process<keyword::glbi, cstore<control::GLBI>>,
              process<keyword::pdfi, cstore<control::PDFI>>,
              process<keyword::stai, cstore<control::STAI>>,
              process<keyword::ttyi, cstore<control::TTYI>>,
              process<keyword::dmpi, cstore<control::DMPI>>
            > {};

  // common to all RNG test suites
  struct rngtest_common :
         sor< process<keyword::suite, store_rngtest>
            > {};

  // hommix block
  struct hommix :
         ifmust< parse<keyword::hommix, store_physics>,
                 block< geometry, physics_common,
                        dir, gendir, statistics > > {};

  // homrt block
  struct homrt :
         ifmust< parse<keyword::homrt, store_physics>,
                 block< geometry, physics_common,
                        dir, gendir, slm, beta, statistics > > {};

  // homhydro block
  struct homhydro :
         ifmust< parse<keyword::homhydro, store_physics>,
                 block< geometry, physics_common,
                        slm, freq_gamma, statistics > > {};

  // spinsflow block
  struct spinsflow :
         ifmust< parse<keyword::spinsflow, store_physics>,
                 block< geometry, physics_common,
                        slm, freq_gamma, dir, gendir, beta > > {};
  // rngtest block
  struct rngtest :
         ifmust< parse<keyword::rngtest, store_physics>,
                 block< rngtest_common >
               > {};

  // physics
  struct physics :
         sor< hommix,
              homhydro,
              homrt,
              spinsflow,
              rngtest > {};

  // main keywords
  struct keywords :
         sor< title,
              physics > {};

  // ignore: comments and empty lines
  struct ignore :
         sor< comment, until<eol, space> > {};

  // entry point: parse keywords and ignores until eof
  struct read_file :
         until< eof, sor<keywords, ignore> > {};

} // namespace grammar

} // namespace Quinoa

#endif // Grammar_h
