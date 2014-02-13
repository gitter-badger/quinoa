//******************************************************************************
/*!
  \file      src/Control/Quinoa/InputDeck/Grammar.h
  \author    J. Bakosi
  \date      Tue 11 Feb 2014 09:08:22 PM CET
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     Quinoa's input deck grammar definition
  \details   Quinoa's input deck grammar definition. We use the Parsing
  Expression Grammar Template Library (PEGTL) to create the grammar and the
  associated parser. Credit goes to Colin Hirsch (pegtl@cohi.at) for PEGTL. Word
  of advice: read from the bottom up.
*/
//******************************************************************************
#ifndef QuinoaInputDeckGrammar_h
#define QuinoaInputDeckGrammar_h

#include <Macro.h>
#include <Exception.h>
#include <Option.h>
#include <PEGTLParsed.h>
#include <Quinoa/Types.h>
#include <Quinoa/InputDeck/Keywords.h>
#include <Grammar.h>
#include <MKLGrammar.h>
#include <RNGSSEGrammar.h>

namespace quinoa {
namespace deck {

  //! PEGTLParsed type specialized to Quinoa's input deck parser
  using PEGTLInputDeck = ctr::PEGTLParsed< ctr::InputDeck,
                                           pegtl::file_input< ctr::Location >,
                                           tag::cmd,
                                           ctr::CmdLine >;

  // Quinoa's InputDeck state

  //! Everything is stored in Stack during parsing
  using Stack = PEGTLInputDeck;
  //! Out-of-struct storage of field ID for pushing terms for statistics
  static int field = 0;

  // Quinoa's InputDeck actions

  //! start new product in vector of statistics
  struct start_product : pegtl::action_base< start_product > {
    static void apply(const std::string& value, Stack& stack) {
      stack.push_back< tag::stat >( ctr::Product() );
      IGNORE(value);   // suppress compiler warning: parameter never referenced
    }
  };

  //! add matched value as Term into vector of Product in vector of statistics
  template< ctr::Quantity q, ctr::Moment m, char name='\0' >
  struct push_term : pegtl::action_base< push_term< q, m, name > > {
    static void apply( const std::string& value, Stack& stack ) {
      // If name is given, push name, otherwise push first char of value
      char na(name ? name : value[0]);
      // If name is given, it is triggered not user-requested
      bool plot(name ? false : true);
      // Use stats for shorthand of reference to stats vector
      std::vector< ctr::Product >& stats = stack.get< tag::stat >();
      // Push term into current product
      stats.back().push_back( ctr::Term( field, q, m, na, plot ) );
      // If central moment, trigger mean
      if (m == ctr::Moment::CENTRAL) {
        ctr::Term term( field, q, ctr::Moment::ORDINARY, toupper(na), false );
        stats.insert( stats.end() - 1, ctr::Product( 1, term ) );
      }
      field = 0;            // reset default field
    }
  };

  //! save field ID so push_term can pick it up
  struct save_field : pegtl::action_base< save_field > {
    static void apply(const std::string& value, Stack& stack) {
      field = stack.convert< int >( value ) - 1;  // field ID numbers start at 0
    }
  };

  //! put option in state at position given by tags
  template< class OptionType, typename... tags >
  struct store_option : pegtl::action_base< store_option<OptionType,tags...> > {
    static void apply(const std::string& value, Stack& stack) {
      tk::grm::store_option< Stack, OptionType, ctr::InputDeck, tags... >
                           ( stack, value, ctr::InputDeckDefaults );
    }
  };

  //! put option in state at position given by tags if among the selected
  template< class OptionType, typename sel, typename vec, typename... tags >
  struct check_store_option :
  pegtl::action_base< check_store_option< OptionType, sel, vec, tags... > >
  {
    static void apply(const std::string& value, Stack& stack) {
      // error out if chosen item does not exist in selected vector
      tk::Option< OptionType > opt;
      bool exists = false;
      for (const auto& r : stack.template get< sel, vec >()) {
        if (opt.value(value) == r) exists = true;
      }
      if (exists) {
        tk::grm::store_option< Stack, OptionType, ctr::InputDeck, tags... >
                             ( stack, value, ctr::InputDeckDefaults );
      } else {
        tk::grm::handleError< Stack, tk::grm::Error::NOTSELECTED >
                            ( stack, value );
      }
    }
  };

  // Quinoa's InputDeck grammar

  //! moment: 'keyword' optionally followed by a digit, pushed to vector of terms
  template< class keyword, ctr::Quantity q, ctr::Moment m >
  struct moment :
         pegtl::sor < pegtl::ifapply< pegtl::seq< keyword,
                                                  pegtl::ifapply< pegtl::digit,
                                                                  save_field > >,
                                      push_term< q, m > >,
                      pegtl::ifapply< keyword, push_term< q, m > > > {};

  //! terms recognized within an expectation and their mapping
  struct terms :
         pegtl::sor< moment< kw::transported_scalar::pegtl_string,
                             ctr::Quantity::SCALAR,
                             ctr::Moment::ORDINARY >,
                     moment< kw::transported_scalar_fluctuation::pegtl_string,
                             ctr::Quantity::SCALAR,
                             ctr::Moment::CENTRAL >,
                     moment< kw::velocity_x::pegtl_string,
                             ctr::Quantity::VELOCITY_X,
                             ctr::Moment::ORDINARY >,
                     tk::grm::unknown< Stack, tk::grm::Error::MOMENT > > {};

  //! plow through terms in expectation until character 'rbound'
  template< char rbound >
  struct expectation :
         pegtl::until< pegtl::one< rbound >, terms > {};

  //! plow through expectations between characters 'lbound' and 'rbound'
  template< char lbound, char rbound >
  struct parse_expectations :
         tk::grm::readkw< pegtl::ifmust< pegtl::one< lbound >,
                                         pegtl::apply< start_product >,
                                         expectation< rbound > > > {};

  //! control parameter
  template< typename keyword, typename... tags >
  struct control :
         tk::grm::process< Stack,
                           typename keyword::pegtl_string,
                           tk::grm::Store< Stack, tags... > > {};

  //! incrementation control parameter
  template< typename keyword, typename Tag >
  struct incpar :
         control< keyword, tag::incpar, Tag > {};

  //! component control parameter
  template< typename keyword, typename Tag >
  struct component :
         control< keyword, tag::component, Tag > {};

  //! interval control parameter
  template< typename keyword, typename Tag >
  struct interval :
         control< keyword, tag::interval, Tag > {};

  //! model parameter
  template< typename keyword, typename model, typename Tag >
  struct parameter :
         control< keyword, tag::param, model, Tag > {};

  //! model parameter vector
  template< typename keyword, typename...tags >
  struct parameter_vector :
         tk::grm::vector< Stack,
                          typename keyword::pegtl_string,
                          tk::grm::Store_back< Stack, tag::param, tags... > > {};

  //! rng parameter
  template< typename keyword, typename option, typename model, typename... tags >
  struct rng :
         tk::grm::process< Stack,
                           typename keyword::pegtl_string,
                           check_store_option< option,
                                               tag::selected,
                                               tk::tag::rng,
                                               tag::param, model, tags... >,
                           pegtl::alpha > {};

  //! scan selected option
  template< typename keyword, typename option, typename... tags >
  struct select_option :
         tk::grm::scan< typename keyword::pegtl_string,
                        store_option< option, tag::selected, tags... > > {};

  //! scan selected option and trigger
  template< typename keyword, typename option, typename Tag,
            typename... triggers >
  struct select_option_and_trigger :
         tk::grm::scan< typename keyword::pegtl_string,
                        store_option< option, tag::selected, Tag >,
                        triggers... > {};

  //! scan and store geometry keyword and option
  template< typename keyword >
  struct scan_geometry :
         select_option< keyword, ctr::Geometry, tag::geometry > {};

  //! scan and store MonteCarlo keyword and option
  template< typename keyword >
  struct scan_montecarlo :
         select_option< keyword, ctr::MonteCarlo, tag::montecarlo > {};

  //! scan and store mass keyword and option
  template< typename keyword >
  struct scan_mass :
         select_option< keyword, ctr::Mass, tag::mass > {};

  //! scan and store hydro keyword and option
  template< typename keyword >
  struct scan_hydro :
         select_option< keyword, ctr::Hydro, tag::hydro > {};

  //! scan and store mix keyword and option
  template< typename keyword >
  struct scan_mix :
         select_option< keyword, ctr::Mix, tag::mix > {};

  //! scan and store frequency keyword and option
  template< typename keyword >
  struct scan_frequency :
         select_option< keyword, ctr::Frequency, tag::frequency > {};

  //! scan and store_back sde keyword and option
  template< typename keyword >
  struct scan_sde :
         tk::grm::scan< typename keyword::pegtl_string,
                        tk::grm::store_back_option< Stack,
                                                    ctr::SDE,
                                                    tag::selected,
                                                    tag::sde > > {};

  //! title
  struct title :
         pegtl::ifmust< tk::grm::readkw< tk::kw::title::pegtl_string >,
                                         tk::grm::quoted<
                                           Stack,
                                           tk::grm::Set< Stack,
                                                         tag::title > > > {};

  //! analytic_geometry block
  struct analytic_geometry:
         pegtl::ifmust< scan_geometry< kw::analytic_geometry >,
                        tk::grm::block< Stack > > {};

  //! discrete_geometry block
  struct discrete_geometry:
         pegtl::ifmust< scan_geometry< kw::discrete_geometry >,
                        tk::grm::block< Stack > > {};

  //! statistics block
  struct statistics :
         pegtl::ifmust< tk::grm::readkw< kw::statistics::pegtl_string >,
                        tk::grm::block< Stack, parse_expectations<'<','>'> > > {};

  //! Fluctuating velocity in x direction
  struct u :
         push_term< ctr::Quantity::VELOCITY_X,
                    ctr::Moment::CENTRAL, 'u' > {};

  //! Fluctuating velocity in y direction
  struct v :
         push_term< ctr::Quantity::VELOCITY_Y,
                    ctr::Moment::CENTRAL, 'v' > {};

  //! Fluctuating velocity in z direction
  struct w :
         push_term< ctr::Quantity::VELOCITY_Z,
                    ctr::Moment::CENTRAL, 'w' > {};

  //! slm block
  struct slm :
         pegtl::ifmust<
           select_option_and_trigger< kw::hydro_slm,
                                      ctr::Hydro,
                                      tag::hydro,
                                      // trigger Reynolds-stress diagonal
                                      start_product, u, u,
                                      start_product, v, v,
                                      start_product, w, w >,
           tk::grm::block< Stack,
                           parameter< kw::SLM_C0, tag::slm, tag::c0 >,
                           component< kw::nvelocity, tag::nvelocity > > > {};

  //! dirichlet mix model block
  struct mix_dir :
         pegtl::ifmust< scan_mix< kw::mix_dir >,
                        tk::grm::block< Stack,
                                        component< kw::nscalar, tag::nscalar >,
                                        rng< kw::rng,
                                             tk::ctr::RNG,
                                             tag::dirichlet,
                                             tk::tag::rng >,
                                        parameter_vector< kw::sde_b,
                                                          tag::dirichlet,
                                                          tag::b >,
                                        parameter_vector< kw::sde_S,
                                                          tag::dirichlet,
                                                          tag::S >,
                                        parameter_vector< kw::sde_kappa,
                                                          tag::dirichlet,
                                                          tag::kappa > > > {};

  //! Generalized Dirichlet mix model block
  struct mix_gendir :
         pegtl::ifmust< scan_mix< kw::mix_gendir >,
                        tk::grm::block< Stack,
                                        component< kw::nscalar, tag::nscalar >,
                                        rng< kw::rng,
                                             tk::ctr::RNG,
                                             tag::gendir,
                                             tk::tag::rng >,
                                        parameter_vector< kw::sde_b,
                                                          tag::gendir,
                                                          tag::b >,
                                        parameter_vector< kw::sde_S,
                                                          tag::gendir,
                                                          tag::S >,
                                        parameter_vector< kw::sde_kappa,
                                                          tag::gendir,
                                                          tag::kappa >,
                                        parameter_vector< kw::sde_c,
                                                          tag::gendir,
                                                          tag::c > > > {};

  //! Gamma turbulence frequency model block
  struct freq_gamma :
         pegtl::ifmust< scan_frequency< kw::freq_gamma >,
                        tk::grm::block<
                          Stack,
                          component< kw::nfreq, tag::nfrequency >,
                          rng< kw::rng, tk::ctr::RNG, tag::gamma, tk::tag::rng >,
                          parameter< kw::freq_gamma_C1, tag::gamma, tag::c1 >,
                          parameter< kw::freq_gamma_C2, tag::gamma, tag::c2 >,
                          parameter< kw::freq_gamma_C3, tag::gamma, tag::c3 >,
                          parameter< kw::freq_gamma_C4, tag::gamma, tag::c4 > >
                      > {};

  //! Beta mass model block
  struct mass_beta :
         pegtl::ifmust< scan_mass< kw::mass_beta >,
                        tk::grm::block<
                          Stack,
                          component< kw::ndensity, tag::ndensity >,
                          rng< kw::rng, tk::ctr::RNG, tag::beta, tk::tag::rng >,
                          parameter< kw::Beta_At, tag::beta, tag::atwood > >
                      > {};

  //! geometry definition types
  struct geometry :
         pegtl::sor< analytic_geometry,
                     discrete_geometry > {};

  //! common to all monte-carlo
  struct montecarlo_common :
         pegtl::sor< control< kw::npar, tag::param, tag::npar >,
                     incpar< kw::nstep, tag::nstep >,
                     incpar< kw::term,  tag::term >,
                     incpar< kw::dt, tag::dt >,
                     interval< kw::glbi, tag::glob >,
                     interval< kw::pdfi, tag::pdf >,
                     interval< kw::stai, tag::plot >,
                     interval< kw::ttyi, tag::tty >,
                     interval< kw::dmpi, tag::dump > > {};

  //! rngs
  struct rngs :
         pegtl::sor< tk::mkl::rngs< Stack,
                                    tag::selected, tk::tag::rng,
                                    tag::param, tk::tag::mklrng >,
                     tk::rngsse::rngs< Stack,
                                       tag::selected, tk::tag::rng,
                                       tag::param, tk::tag::rngsse > > {};

  // RNGs block
  struct rngblock :
         pegtl::ifmust< tk::grm::readkw< kw::rngs::pegtl_string >,
                        tk::grm::block< Stack, rngs > > {};

  //! mass models
  struct mass :
         pegtl::sor< mass_beta > {};

  //! hydro models
  struct hydro :
         pegtl::sor< slm > {};

  //! material mix models
  struct mix :
         pegtl::sor< mix_dir, mix_gendir > {};

  //! turbulence frequency models
  struct freq :
         pegtl::sor< freq_gamma > {};

  //! MonteCarlo physics 'hommix' block
  struct hommix :
         pegtl::ifmust< scan_montecarlo< kw::hommix >,
                        tk::grm::block< Stack,
                                        geometry,
                                        montecarlo_common,
                                        mix,
                                        rngblock,
                                        statistics > > {};

  //! MonteCarlo physics 'homrt' block
  struct homrt :
         pegtl::ifmust< scan_montecarlo< kw::homrt >,
                        tk::grm::block< Stack,
                                        geometry,
                                        montecarlo_common,
                                        mass,
                                        hydro,
                                        freq,
                                        rngblock,
                                        statistics > > {};

  //! MonteCarlo physics 'homhydro' block
  struct homhydro :
         pegtl::ifmust< scan_montecarlo< kw::homhydro >,
                        tk::grm::block< Stack,
                                        geometry,
                                        montecarlo_common,
                                        hydro,
                                        freq,
                                        rngblock,
                                        statistics > > {};

  //! MonteCarlo physics 'spinsflow' block
  struct spinsflow :
         pegtl::ifmust< scan_montecarlo< kw::spinsflow >,
                        tk::grm::block< Stack,
                                        geometry,
                                        montecarlo_common,
                                        hydro,
                                        freq,
                                        mix,
                                        rngblock,
                                        statistics > > {};

  //! policy parameter
  template< typename keyword, typename option, typename sde, typename... tags >
  struct policy :
         tk::grm::process< Stack,
                           typename keyword::pegtl_string,
                           store_option< option, tag::param, sde, tags... >,
                           pegtl::alpha > {};


  //! Ornstein-Uhlenbeck SDE
  struct ornstein_uhlenbeck :
         pegtl::ifmust< scan_sde< kw::ornstein_uhlenbeck >,
                        tk::grm::block< Stack,
                                        component< kw::ncomp, tag::nou >,
                                        rng< kw::rng,
                                             tk::ctr::RNG,
                                             tag::ou,
                                             tk::tag::rng > > > {};

  //! Log-normal SDE
  struct lognormal :
         pegtl::ifmust< scan_sde< kw::lognormal >,
                        tk::grm::block< Stack,
                                        component< kw::ncomp, tag::nlognormal >,
                                        rng< kw::rng,
                                             tk::ctr::RNG,
                                             tag::ou,
                                             tk::tag::rng > > > {};

  //! Skew-normal SDE
  struct skewnormal :
         pegtl::ifmust< scan_sde< kw::skewnormal >,
                        tk::grm::block< Stack,
                                        component< kw::ncomp, tag::nskewnormal >,
                                        rng< kw::rng,
                                             tk::ctr::RNG,
                                             tag::ou,
                                             tk::tag::rng > > > {};

  //! Gamma SDE
  struct gamma :
         pegtl::ifmust< scan_sde< kw::gamma >,
                        tk::grm::block< Stack,
                                        component< kw::ncomp, tag::ngamma >,
                                        rng< kw::rng,
                                             tk::ctr::RNG,
                                             tag::gamma,
                                             tk::tag::rng > > > {};

  //! Beta SDE
  struct beta :
         pegtl::ifmust< scan_sde< kw::beta >,
                        tk::grm::block< Stack,
                                        component< kw::ncomp, tag::nbeta >,
                                        rng< kw::rng,
                                             tk::ctr::RNG,
                                             tag::beta,
                                             tk::tag::rng >,
                                        parameter< kw::sde_b, tag::beta, tag::b >,
                                        parameter< kw::sde_S, tag::beta, tag::S >,
                                        parameter< kw::sde_kappa,
                                                   tag::beta,
                                                   tag::kappa > > > {};

  //! Dirichlet SDE
  struct dirichlet :
         pegtl::ifmust< scan_sde< kw::dirichlet >,
                        tk::grm::block< Stack,
                                        component< kw::ncomp, tag::ndirichlet >,
                                        rng< kw::rng,
                                             tk::ctr::RNG,
                                             tag::dirichlet,
                                             tk::tag::rng >,
                                        policy< kw::init,
                                                ctr::InitPolicy,
                                                tag::dirichlet,
                                                tag::initpolicy >,
                                        policy< kw::coeff,
                                                ctr::CoeffPolicy,
                                                tag::dirichlet,
                                                tag::coeffpolicy >,
                                        parameter_vector< kw::sde_b,
                                                          tag::dirichlet,
                                                          tag::b >,
                                        parameter_vector< kw::sde_S,
                                                          tag::dirichlet,
                                                          tag::S >,
                                        parameter_vector< kw::sde_kappa,
                                                          tag::dirichlet,
                                                          tag::kappa > > > {};
  //! Generalized Dirichlet SDE
  struct generalized_dirichlet :
         pegtl::ifmust< scan_sde< kw::gendir >,
                        tk::grm::block< Stack,
                                        component< kw::ncomp, tag::ngendir >,
                                        rng< kw::rng,
                                             tk::ctr::RNG,
                                             tag::gendir,
                                             tk::tag::rng >,
                                        policy< kw::init,
                                                ctr::InitPolicy,
                                                tag::gendir,
                                                tag::initpolicy >,
                                        policy< kw::coeff,
                                                ctr::CoeffPolicy,
                                                tag::gendir,
                                                tag::coeffpolicy >,
                                        parameter_vector< kw::sde_b,
                                                          tag::gendir,
                                                          tag::b >,
                                        parameter_vector< kw::sde_S,
                                                          tag::gendir,
                                                          tag::S >,
                                        parameter_vector< kw::sde_kappa,
                                                          tag::gendir,
                                                          tag::kappa >,
                                        parameter_vector< kw::sde_c,
                                                          tag::gendir,
                                                          tag::c > > > {};

  //! stochastic differential equations
  struct sde :
         pegtl::sor< dirichlet,
                     generalized_dirichlet,
                     ornstein_uhlenbeck,
                     lognormal,
                     skewnormal,
                     gamma,
                     beta > {};

  //! 'testsde' block
  struct testsde :
         pegtl::ifmust< scan_montecarlo< kw::testsde >,
                        tk::grm::block< Stack,
                                        montecarlo_common,
                                        sde,
                                        rngblock,
                                        statistics > > {};

  //! montecarlo physics types
  struct physics :
         pegtl::sor< hommix,
                     homhydro,
                     homrt,
                     spinsflow > {};

  //! montecarlo types
  struct montecarlo :
         pegtl::sor< testsde, physics > {};

  //! main keywords
  struct keywords :
         pegtl::sor< title, montecarlo > {};

  //! ignore: comments and empty lines
  struct ignore :
         pegtl::sor< tk::grm::comment,
                     pegtl::until< pegtl::eol, pegtl::space > > {};

  //! entry point: parse keywords and ignores until eof
  struct read_file :
         tk::grm::read_file< Stack, keywords, ignore > {};

} // deck::
} // quinoa::

#endif // QuinoaInputDeckGrammar_h
