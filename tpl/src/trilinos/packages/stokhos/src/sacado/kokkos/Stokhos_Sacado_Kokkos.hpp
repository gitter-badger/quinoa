// @HEADER
// ***********************************************************************
//
//                           Stokhos Package
//                 Copyright (2009) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Eric T. Phipps (etphipp@sandia.gov).
//
// ***********************************************************************
// @HEADER

#ifndef STOKHOS_SACADO_KOKKOS_HPP
#define STOKHOS_SACADO_KOKKOS_HPP

#include "Stokhos_ConfigDefs.h"

#ifdef HAVE_STOKHOS_KOKKOSCORE

#include "KokkosCore_config.h"

#include "Stokhos_Sacado_Kokkos_MathFunctions.hpp"

#include "Stokhos_KokkosTraits.hpp"
#include "Stokhos_StaticFixedStorage.hpp"
#include "Stokhos_StaticStorage.hpp"
#include "Stokhos_DynamicStorage.hpp"
#include "Stokhos_DynamicStridedStorage.hpp"
#include "Stokhos_DynamicThreadedStorage.hpp"
#include "Stokhos_LocalStorage.hpp"
#include "Stokhos_ViewStorage.hpp"
#include "Stokhos_ViewStridedStorage.hpp"

#include "Sacado_MP_ExpressionTraits.hpp"
#include "Sacado_MP_VectorTraits.hpp"

#include "Sacado_MP_Vector.hpp"
#include "Kokkos_View_MP_Vector.hpp"
#include "Kokkos_Atomic_MP_Vector.hpp"

#endif // HAVE_STOKHOS_KOKKOSCORE

#endif // STOKHOS_SACADO_KOKKOS_HPP