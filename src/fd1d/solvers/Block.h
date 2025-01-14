#ifndef FD1D_BLOCK_H
#define FD1D_BLOCK_H

/*
* PSCF - Polymer Self-Consistent Field Theory
*
* Copyright 2016 - 2019, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include "Propagator.h"                   // base class argument
#include <fd1d/domain/GeometryMode.h>     // argument (enum)
#include <pscf/solvers/BlockTmpl.h>       // base class template
#include <pscf/math/TridiagonalSolver.h>  // member

namespace Pscf { 
namespace Fd1d 
{ 

   class Domain;
   using namespace Util;

   /**
   * Block within a branched polymer.
   *
   * Derived from BlockTmpl<Propagator>. A BlockTmpl<Propagator> has two 
   * Propagator members and is derived from BlockDescriptor.
   *
   * \ingroup Fd1d_Solver_Module
   */
   class Block : public BlockTmpl<Propagator>
   {

   public:

      /**
      * Generic field (base class)
      */
      typedef Propagator::Field Field;

      /**
      * Monomer chemical potential field.
      */
      typedef Propagator::WField WField;

      /**
      * Constrained partition function q(r,s) for fixed s.
      */
      typedef Propagator::QField QField;

      // Member functions

      /**
      * Constructor.
      */
      Block();

      /**
      * Destructor.
      */
      ~Block();

      /**
      * Initialize discretization and allocate required memory.
      *
      * \param domain associated Domain object, with grid info
      * \param ds desired (optimal) value for contour length step
      */
      void setDiscretization(Domain const & domain, double ds);

      /**
      * Set length and readjust ds_ accordingly.
      *
      * \param length  length (# of monomers) for this block
      */
      virtual void setLength(double length);

      /**
      * Set Crank-Nicholson solver for this block.
      *
      * \param w  Chemical potential field (input)
      */
      void setupSolver(WField const & w);

      /**
      * Compute unnormalized concentration for block by integration.
      *
      * Upon return, grid point r of array cField() contains the 
      * integral int ds q(r,s)q^{*}(r,L-s) times the prefactor, 
      * where q(r,s) is the solution obtained from propagator(0), 
      * and q^{*} is the solution of propagator(1),  and s is 
      * a contour variable that is integrated over the domain 
      * 0 < s < length(), where length() is the block length.
      *
      * \param prefactor constant prefactor multiplying integral
      */ 
      void computeConcentration(double prefactor);

      /**
      * Compute one step of integration loop, from i to i+1.
      *
      * \param q  QField at step i (input)
      * \param qNew  QField at step i + 1 (output)
      */
      void step(QField const & q, QField& qNew);

      /**
      * Return associated domain by reference.
      */
      Domain const & domain() const;

      /**
      * Number of contour length steps.
      */
      int ns() const;

   private:
 
      /// Solver used in Crank-Nicholson algorithm
      TridiagonalSolver solver_;

      // Arrays dA_, uA_, lB_ dB_, uB_, luB_ contain elements of the 
      // the tridiagonal matrices A and B used in propagation from
      // step i to i + 1, which requires solution of a linear system 
      // of the form: A q(i+1) = B q(i).

      /// Diagonal elements of matrix A
      DArray<double> dA_;

      /// Off-diagonal upper elements of matrix A
      DArray<double> uA_;

      /// Off-diagonal lower elements of matrix A
      DArray<double> lA_;

      /// Diagonal elements of matrix B
      DArray<double> dB_;

      /// Off-diagonal upper elements of matrix B
      DArray<double> uB_;

      /// Off-diagonal lower elements of matrix B
      DArray<double> lB_;

      /// Work vector
      DArray<double> v_;

      /// Pointer to associated Domain object.
      Domain const * domainPtr_;

      /// Contour length step size.
      double ds_;

      /// Number of contour length steps = # grid points - 1.
      int ns_;

   };

   // Inline member functions

   /// Get Domain by reference.
   inline Domain const & Block::domain() const
   {   
      UTIL_ASSERT(domainPtr_);
      return *domainPtr_;
   }

   /// Get number of contour steps.
   inline int Block::ns() const
   {  return ns_; }

}
}
#endif
