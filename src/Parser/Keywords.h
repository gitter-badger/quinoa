//******************************************************************************
/*!
  \file      src/Parser/Keywords.h
  \author    J. Bakosi
  \date      Sat 30 Mar 2013 12:16:09 PM MDT
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     Keywords
  \details   All keywords recognized by the parser
*/
//******************************************************************************
#ifndef Grammar_h
#error "Keywords.h should only be included within Grammar.h"
#endif

#ifndef Keywords_h
#define Keywords_h

// Keywords accepted by the parser
namespace keyword {

  // Editing anything below should be accompanied by the corresponding changes
  // in FwdAssociate.h and BackAssociate.h in src/Control.

  // Problem title
  using title = pegtl::string<t,i,t,l,e>;

  // End of block
  using end = pegtl::string< e,n,d >;

  // Select physics:
  //   * Homogeneous material mixing
  using hommix = pegtl::string< h,o,m,m,i,x >;
  //   * Homogeneous hydrodinamics
  using homhydro = pegtl::string< h,o,m,h,y,d,r,o >;
  //   * Standalone-particle incompressible Navier-Stokes flow
  using spinsflow = pegtl::string< s,p,i,n,s,f,l,o,w >;

  // Select hydrodynamics model:
  //   * Simplified Langevin model
  using slm = pegtl::string< s,l,m >;
  //   * Generalized Langevin model
  using glm = pegtl::string< g,l,m >;

  // Select material mix model:
  //   * Interaction by exchange with the mean
  using iem = pegtl::string< i,e,m >;
  //   * Interaction by exchange with the conditional mean
  using iecm = pegtl::string< i,e,c,m >;
  //   * Dirichlet
  using dir = pegtl::string< d,i,r >;
  //   * generalized Dirichlet
  using gendir = pegtl::string< g,e,n,d,i,r >;

  // Number of time steps to take
  using nstep = pegtl::string< n,s,t,e,p >;

  // Terminate time stepping at this value
  using term = pegtl::string< t, e, r, m >;

  // Size of time step
  using dt = pegtl::string< d,t >;

  // Start of hydrodynamics model specification block
  using hydro = pegtl::string< h,y,d,r,o >;

  // Start of material mix model specification block
  using mix = pegtl::string< m,i,x >;

  // Number of mixing scalars
  using nscalar = pegtl::string< n,s,c,a,l,a,r >;

  // Dirichlet and generalized Dirichlet parameters
  using B = pegtl::string< b >;
  using S = pegtl::string< S >;
  using kappa = pegtl::string< k,a,p,p,a >;
  using C = pegtl::string< C >;

  // Quantities
  using transported_scalar = pegtl::string< Y >;
  using transported_scalar_fluctuation = pegtl::string< y >;

  using velocity_x = pegtl::string< U >;
  using velocity_fluctuation_x = pegtl::string< u >;
  using velocity_y = pegtl::string< V >;
  using velocity_fluctuation_y = pegtl::string< v >;
  using velocity_z = pegtl::string< W >;
  using velocity_fluctuation_z = pegtl::string< w >;

  using pressure = pegtl::string< P >;
  using pressure_fluctuation = pegtl::string< p >;
  
  using density = pegtl::string< R >;
  using density_fluctuation = pegtl::string< r >;
  
  // Total number of particles
  using npar = pegtl::string< n,p,a,r >;

  // TTY (screen) output interval
  using ttyi = pegtl::string< t,t,y,i >;

  // Dump (restart file) output interval
  using dump = pegtl::string< d,u,m,p >;

  // Plot output interval
  using plti = pegtl::string< p,l,t,i >;

  // PDF output interval
  using pdfi = pegtl::string< p,d,f,i >;

  // Glob output interval
  using glob = pegtl::string< g,l,o,b >;

  // Statistics
  using statistics = pegtl::string< s,t,a,t,i,s,t,i,c,s >;

  // PDF base filename
  using pdfname = pegtl::string< p,d,f,n,a,m,e >;

  // Glob (i.e. domain-average statistics) filename
  using globname = pegtl::string< g,l,o,b,n,a,m,e >;

  // Plot base filename
  using plotname = pegtl::string< p,l,o,t,n,a,m,e >;

} // namespace keyword

#endif // Keywords_h
