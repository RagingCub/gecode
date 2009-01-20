/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *     Guido Tack <tack@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2002
 *     Guido Tack, 2004
 *
 *  Last modified:
 *     $Date$ by $Author$
 *     $Revision$
 *
 *  This file is part of Gecode, the generic constraint
 *  development environment:
 *     http://www.gecode.org
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __GECODE_INT_BOOL_HH__
#define __GECODE_INT_BOOL_HH__

#include <gecode/int.hh>

/**
 * \namespace Gecode::Int::Bool
 * \brief Boolean propagators
 */

namespace Gecode { namespace Int { namespace Bool {

  /*
   * Base Classes
   *
   */

  /// Base-class for binary Boolean propagators
  template<class BVA, class BVB>
  class BoolBinary : public Propagator {
  protected:
    BVA x0; ///< Boolean view
    BVB x1; ///< Boolean view
    /// Constructor for posting
    BoolBinary(Space& home, BVA b0, BVB b1);
    /// Constructor for cloning
    BoolBinary(Space& home, bool share, BoolBinary& p);
    /// Constructor for rewriting \a p during cloning
    BoolBinary(Space& home, bool share, Propagator& p,
               BVA b0, BVB b1);
  public:
    /// Cost function (defined as low unary)
    virtual PropCost cost(const Space& home, const ModEventDelta& med) const;
    /// Delete propagator and return its size
    virtual size_t dispose(Space& home);
  };

  /// Base-class for ternary Boolean propagators
  template<class BVA, class BVB, class BVC>
  class BoolTernary : public Propagator {
  protected:
    BVA x0; ///< Boolean view
    BVB x1; ///< Boolean view
    BVC x2; ///< Boolean view
    /// Constructor for posting
    BoolTernary(Space& home, BVA b0, BVB b1, BVC b2);
    /// Constructor for cloning
    BoolTernary(Space& home, bool share, BoolTernary& p);
  public:
    /// Constructor for rewriting \a p during cloning
    BoolTernary(Space& home, bool share, Propagator& p,
                BVA b0, BVB b1, BVC b2);
    /// Cost function (defined as low binary)
    virtual PropCost cost(const Space& home, const ModEventDelta& med) const;
    /// Delete propagator and return its size
    virtual size_t dispose(Space& home);
  };

  /**
   * \brief Boolean equality propagator
   *
   * Requires \code #include <gecode/int/bool.hh> \endcode
   * \ingroup FuncIntProp
   */
  template<class BVA, class BVB>
  class Eq : public BoolBinary<BVA,BVB> {
    GECODE_REFLECT_PROPAGATOR_2(Eq,BVA,BVB,"Gecode::Int::Bool::Eq")
    GECODE_REFLECT_ARGS_2(BVA,x0,BVB,x1)
  protected:
    using BoolBinary<BVA,BVB>::x0;
    using BoolBinary<BVA,BVB>::x1;
    /// Constructor for posting
    Eq(Space& home, BVA b0, BVB b1);
    /// Constructor for cloning \a p
    Eq(Space& home, bool share, Eq& p);
  public:
    /// Constructor for rewriting \a p during cloning
    Eq(Space& home, bool share, Propagator& p,
       BVA b0, BVB b1);
    /// Copy propagator during cloning
    virtual Actor* copy(Space& home, bool share);
    /// Perform propagation
    virtual ExecStatus propagate(Space& home, const ModEventDelta& med);
    /// Post propagator \f$ x_0 = x_1\f$
    static  ExecStatus post(Space& home, BVA x0, BVB x1);
  };


  /**
   * \brief n-ary Boolean equality propagator
   *
   * Requires \code #include <gecode/int/bool.hh> \endcode
   * \ingroup FuncIntProp
   */
  template<class BV>
  class NaryEq : public NaryPropagator<BV,PC_BOOL_VAL> {
    GECODE_REFLECT_PROPAGATOR_1(NaryEq,BV,"Gecode::Int::Bool::NaryEq")
    GECODE_REFLECT_ARGS_1(ViewArray<BV>,x)
  protected:
    using NaryPropagator<BV,PC_BOOL_VAL>::x;
    /// Constructor for posting
    NaryEq(Space& home, ViewArray<BV>& x);
    /// Constructor for cloning \a p
    NaryEq(Space& home, bool share, NaryEq& p);
  public:
    /// Copy propagator during cloning
    virtual Actor* copy(Space& home, bool share);
    /// Cost function (defined as low unary)
    virtual PropCost cost(const Space& home, const ModEventDelta& med) const;
    /// Perform propagation
    virtual ExecStatus propagate(Space& home, const ModEventDelta& med);
    /// Post propagator \f$ x_0 = x_1=\ldots =x_{|x|-1}\f$
    static  ExecStatus post(Space& home, ViewArray<BV>& x);
  };


  /**
   * \brief Boolean less or equal propagator
   *
   * Requires \code #include <gecode/int/bool.hh> \endcode
   * \ingroup FuncIntProp
   */
  template<class BV>
  class Lq : public BoolBinary<BV,BV> {
    GECODE_REFLECT_PROPAGATOR_1(Lq,BV,"Gecode::Int::Bool::Lq")
    GECODE_REFLECT_ARGS_2(BV,x0,BV,x1)
  protected:
    using BoolBinary<BV,BV>::x0;
    using BoolBinary<BV,BV>::x1;
    /// Constructor for posting
    Lq(Space& home, BV b0, BV b1);
    /// Constructor for cloning \a p
    Lq(Space& home, bool share, Lq& p);
  public:
    /// Copy propagator during cloning
    virtual Actor* copy(Space& home, bool share);
    /// Perform propagation
    virtual ExecStatus propagate(Space& home, const ModEventDelta& med);
    /// Post propagator \f$ b_0 \leq b_1\f$
    static  ExecStatus post(Space& home, BV b0, BV b1);
  };


  /**
   * \brief Boolean less propagator
   *
   * Requires \code #include <gecode/int/bool.hh> \endcode
   * \ingroup FuncIntProp
   */
  template<class BV>
  class Le {
  public:
    /// Post propagator \f$ b_0 < b_1\f$
    static  ExecStatus post(Space& home, BV b0, BV b1);
  };


  /**
   * \brief Binary Boolean disjunction propagator (true)
   *
   * Requires \code #include <gecode/int/bool.hh> \endcode
   * \ingroup FuncIntProp
   */
  template<class BVA, class BVB>
  class BinOrTrue : public BoolBinary<BVA,BVB> {
    GECODE_REFLECT_PROPAGATOR_2(BinOrTrue,BVA,BVB,
                                "Gecode::Int::Bool::BinOrTrue")
    GECODE_REFLECT_ARGS_2(BVA,x0,BVB,x1)
  protected:
    using BoolBinary<BVA,BVB>::x0;
    using BoolBinary<BVA,BVB>::x1;
    /// Constructor for posting
    BinOrTrue(Space& home, BVA b0, BVB b1);
    /// Constructor for cloning \a p
    BinOrTrue(Space& home, bool share, BinOrTrue& p);
  public:
    /// Constructor for rewriting \a p during cloning
    BinOrTrue(Space& home, bool share, Propagator& p,
              BVA b0, BVB b1);
    /// Copy propagator during cloning
    virtual Actor* copy(Space& home, bool share);
    /// Perform propagation
    virtual ExecStatus propagate(Space& home, const ModEventDelta& med);
    /// Post propagator \f$ b_0 \lor b_1 = 1 \f$
    static  ExecStatus post(Space& home, BVA b0, BVB b1);
  };

  /**
   * \brief Ternary Boolean disjunction propagator (true)
   *
   * Requires \code #include <gecode/int/bool.hh> \endcode
   * \ingroup FuncIntProp
   */
  template<class BV>
  class TerOrTrue : public BoolBinary<BV,BV> {
    GECODE_REFLECT_PROPAGATOR_1(TerOrTrue,BV,"Gecode::Int::Bool::TerOrTrue")
    GECODE_REFLECT_ARGS_3(BV,x0,BV,x1,BV,x2)
  protected:
    using BoolBinary<BV,BV>::x0;
    using BoolBinary<BV,BV>::x1;
    /// Boolean view without subscription
    BV x2;
    /// Constructor for posting
    TerOrTrue(Space& home, BV b0, BV b1, BV b2);
    /// Constructor for cloning \a p
    TerOrTrue(Space& home, bool share, TerOrTrue& p);
  public:
    /// Constructor for rewriting \a p during cloning
    TerOrTrue(Space& home, bool share, Propagator& p,
              BV b0, BV b1, BV b2);
    /// Copy propagator during cloning
    virtual Actor* copy(Space& home, bool share);
    /// Perform propagation
    virtual ExecStatus propagate(Space& home, const ModEventDelta& med);
    /// Post propagator \f$ b_0 \lor b_1 \lor b_2 = 1 \f$
    static  ExecStatus post(Space& home, BV b0, BV b1, BV b2);
    /// Delete propagator and return its size
    virtual size_t dispose(Space& home);
  };

  /**
   * \brief Quarternary Boolean disjunction propagator (true)
   *
   * Requires \code #include <gecode/int/bool.hh> \endcode
   * \ingroup FuncIntProp
   */
  template<class BV>
  class QuadOrTrue : public BoolBinary<BV,BV> {
    GECODE_REFLECT_PROPAGATOR_1(QuadOrTrue,BV,"Gecode::Int::Bool::QuadOrTrue")
    GECODE_REFLECT_ARGS_4(BV,x0,BV,x1,BV,x2,BV,x3)
  protected:
    using BoolBinary<BV,BV>::x0;
    using BoolBinary<BV,BV>::x1;
    /// Boolean view without subscription
    BV x2;
    /// Boolean view without subscription
    BV x3;
    /// Constructor for posting
    QuadOrTrue(Space& home, BV b0, BV b1, BV b2, BV b3);
    /// Constructor for cloning \a p
    QuadOrTrue(Space& home, bool share, QuadOrTrue& p);
  public:
    /// Constructor for rewriting \a p during cloning
    QuadOrTrue(Space& home, bool share, Propagator& p,
               BV b0, BV b1, BV b2, BV b3);
    /// Copy propagator during cloning
    virtual Actor* copy(Space& home, bool share);
    /// Perform propagation
    virtual ExecStatus propagate(Space& home, const ModEventDelta& med);
    /// Post propagator \f$ b_0 \lor b_1 \lor b_2 \lor b_3 = 1 \f$
    static  ExecStatus post(Space& home, BV b0, BV b1, BV b2, BV b3);
    /// Delete propagator and return its size
    virtual size_t dispose(Space& home);
  };

  /**
   * \brief Boolean disjunction propagator
   *
   * Requires \code #include <gecode/int/bool.hh> \endcode
   * \ingroup FuncIntProp
   */
  template<class BVA, class BVB, class BVC>
  class Or : public BoolTernary<BVA,BVB,BVC> {
    GECODE_REFLECT_PROPAGATOR_3(Or,BVA,BVB,BVC,"Gecode::Int::Bool::Or")
    GECODE_REFLECT_ARGS_3(BVA,x0,BVB,x1,BVC,x2)
  protected:
    using BoolTernary<BVA,BVB,BVC>::x0;
    using BoolTernary<BVA,BVB,BVC>::x1;
    using BoolTernary<BVA,BVB,BVC>::x2;
    /// Constructor for posting
    Or(Space& home, BVA b0, BVB b1, BVC b2);
    /// Constructor for cloning \a p
    Or(Space& home, bool share, Or& p);
  public:
    /// Constructor for rewriting \a p during cloning
    Or(Space& home, bool share, Propagator& p, BVA b0, BVB b1, BVC b2);
    /// Copy propagator during cloning
    virtual Actor* copy(Space& home, bool share);
    /// Perform propagation
    virtual ExecStatus propagate(Space& home, const ModEventDelta& med);
    /// Post propagator \f$ b_0 \lor b_1 = b_2 \f$
    static  ExecStatus post(Space& home, BVA b0, BVB b1, BVC b2);
  };

  /**
   * \brief Boolean n-ary disjunction propagator
   *
   * Requires \code #include <gecode/int/bool.hh> \endcode
   * \ingroup FuncIntProp
   */
  template<class VX,class VY>
  class NaryOr
    : public MixNaryOnePropagator<VX,PC_BOOL_NONE,VY,PC_BOOL_VAL> {
    GECODE_REFLECT_PROPAGATOR_2(NaryOr,VX,VY,"Gecode::Int::Bool::NaryOr")
    GECODE_REFLECT_ARGS_2(ViewArray<VX>,x,VY,y)
  protected:
    using MixNaryOnePropagator<VX,PC_BOOL_NONE,VY,PC_BOOL_VAL>::x;
    using MixNaryOnePropagator<VX,PC_BOOL_NONE,VY,PC_BOOL_VAL>::y;
    /// The number of views assigned to zero in \a x
    int n_zero;
    /// The advisor council
    Council<Advisor> c;
    /// Constructor for posting
    NaryOr(Space& home,  ViewArray<VX>& x, VY y);
    /// Constructor for cloning \a p
    NaryOr(Space& home, bool share, NaryOr<VX,VY>& p);
  public:
    /// Copy propagator during cloning
    virtual Actor* copy(Space& home, bool share);
    /// Give advice to propagator
    virtual ExecStatus advise(Space& home, Advisor& a, const Delta& d);
    /// Cost function (defined as low unary)
    virtual PropCost cost(const Space& home, const ModEventDelta& med) const;
    /// Perform propagation
    virtual ExecStatus propagate(Space& home, const ModEventDelta& med);
    /// Post propagator \f$ \bigvee_{i=0}^{|x|-1} x_i = y\f$
    static  ExecStatus post(Space& home, ViewArray<VX>& x, VY y);
    /// Delete propagator and return its size
    virtual size_t dispose(Space& home);
  };


  /**
   * \brief Boolean n-ary disjunction propagator (true)
   *
   * Requires \code #include <gecode/int/bool.hh> \endcode
   * \ingroup FuncIntProp
   */
  template<class BV>
  class NaryOrTrue : public BinaryPropagator<BV,PC_BOOL_VAL> {
    GECODE_REFLECT_PROPAGATOR_1(NaryOrTrue,BV,"Gecode::Int::Bool::NaryOrTrue")
    GECODE_REFLECT_ARGS_3(BV,x0,BV,x1,ViewArray<BV>,x)
  protected:
    using BinaryPropagator<BV,PC_BOOL_VAL>::x0;
    using BinaryPropagator<BV,PC_BOOL_VAL>::x1;
    /// Views not yet subscribed to
    ViewArray<BV> x;
    /// Update subscription
    ExecStatus resubscribe(Space& home, BV& x0, BV x1);
    /// Constructor for posting
    NaryOrTrue(Space& home, ViewArray<BV>& x);
    /// Constructor for cloning \a p
    NaryOrTrue(Space& home, bool share, NaryOrTrue<BV>& p);
    /// Post propagator \f$ \bigvee_{i=0}^{|b|-1} b_i = 0\f$
    static  ExecStatus post(Space& home, BV x0, BV x1, ViewArray<BV>& b);
  public:
    /// Copy propagator during cloning
    virtual Actor* copy(Space& home, bool share);
    /// Cost function (defined as low unary)
    virtual PropCost cost(const Space& home, const ModEventDelta& med) const;
    /// Perform propagation
    virtual ExecStatus propagate(Space& home, const ModEventDelta& med);
    /// Post propagator \f$ \bigvee_{i=0}^{|b|-1} b_i = 0\f$
    static  ExecStatus post(Space& home, ViewArray<BV>& b);
    /// Delete propagator and return its size
    virtual size_t dispose(Space& home);
  };


  /**
   * \brief Boolean equivalence propagator
   *
   * Requires \code #include <gecode/int/bool.hh> \endcode
   * \ingroup FuncIntProp
   */
  template<class BVA, class BVB, class BVC>
  class Eqv : public BoolTernary<BVA,BVB,BVC> {
    GECODE_REFLECT_PROPAGATOR_3(Eqv,BVA,BVB,BVC,"Gecode::Int::Bool::Eqv")
    GECODE_REFLECT_ARGS_3(BVA,x0,BVB,x1,BVC,x2)
  protected:
    using BoolTernary<BVA,BVB,BVC>::x0;
    using BoolTernary<BVA,BVB,BVC>::x1;
    using BoolTernary<BVA,BVB,BVC>::x2;
    /// Constructor for cloning \a p
    Eqv(Space& home, bool share, Eqv& p);
    /// Constructor for posting
    Eqv(Space& home, BVA b0 ,BVB b1, BVC b2);
  public:
    /// Copy propagator during cloning
    virtual Actor* copy(Space& home, bool share);
    /// Perform propagation
    virtual ExecStatus propagate(Space& home, const ModEventDelta& med);
    /// Post propagator \f$ b_0 \Leftrightarrow b_1 = b_2 \f$ (equivalence)
    static  ExecStatus post(Space& home, BVA b0, BVB b1, BVC b2);
  };


  /**
   * \brief Boolean clause propagator (disjunctive)
   *
   * Requires \code #include <gecode/int/bool.hh> \endcode
   * \ingroup FuncIntProp
   */
  template<class VX, class VY>
  class Clause : public Propagator {
    GECODE_REFLECT_PROPAGATOR_2(Clause,VX,VY,"Gecode::Int::Bool::Clause")
    GECODE_REFLECT_ARGS_3(ViewArray<VX>,x,ViewArray<VY>,y,VX,z)
  protected:
    /// Positive views
    ViewArray<VX> x;
    /// Positive views (origin from negative variables)
    ViewArray<VY> y;
    /// Result
    VX z;
    /// The number of views assigned to zero in \a x and \a y
    int n_zero;
    /// Advisors for views (tagged whether for \a x or \a y)
    class Tagged : public Advisor {
    public:
      /// Whether advises a view for x or y
      const bool x;
      /// Create tagged advisor
      Tagged(Space& home, Propagator& p, Council<Tagged>& c, bool x);
      /// Clone tagged advisor \a a
      Tagged(Space& home, bool share, Tagged& a);
    };
    /// The advisor council
    Council<Tagged> c;
    /// Cancel subscriptions
    void cancel(Space& home);
    /// Constructor for posting
    Clause(Space& home,  ViewArray<VX>& x, ViewArray<VY>& y, VX z);
    /// Constructor for cloning \a p
    Clause(Space& home, bool share, Clause<VX,VY>& p);
  public:
    /// Copy propagator during cloning
    virtual Actor* copy(Space& home, bool share);
    /// Give advice to propagator
    virtual ExecStatus advise(Space& home, Advisor& a, const Delta& d);
    /// Cost function (defined as low unary)
    virtual PropCost cost(const Space& home, const ModEventDelta& med) const;
    /// Perform propagation
    virtual ExecStatus propagate(Space& home, const ModEventDelta& med);
    /// Post propagator \f$ \bigvee_{i=0}^{|x|-1} x_i \vee \bigvee_{i=0}^{|x|-1} y_i = z\f$
    static  ExecStatus post(Space& home, ViewArray<VX>& x, ViewArray<VY>& y,
                            VX z);
    /// Delete propagator and return its size
    virtual size_t dispose(Space& home);
  };


  /**
   * \brief Boolean clause propagator (disjunctive, true)
   *
   * Requires \code #include <gecode/int/bool.hh> \endcode
   * \ingroup FuncIntProp
   */
  template<class VX, class VY>
  class ClauseTrue
    : public MixBinaryPropagator<VX,PC_BOOL_VAL,VY,PC_BOOL_VAL> {
    GECODE_REFLECT_PROPAGATOR_2(ClauseTrue,VX,VY,
                                "Gecode::Int::Bool::ClauseTrue")
    GECODE_REFLECT_ARGS_4(VX,x0,VY,x1,ViewArray<VX>,x,ViewArray<VY>,y)
  protected:
    using MixBinaryPropagator<VX,PC_BOOL_VAL,VY,PC_BOOL_VAL>::x0;
    using MixBinaryPropagator<VX,PC_BOOL_VAL,VY,PC_BOOL_VAL>::x1;
    /// Views not yet subscribed to
    ViewArray<VX> x;
    /// Views not yet subscribed to (origin from negative variables)
    ViewArray<VY> y;
    /// Update subscription
    //    ExecStatus resubscribe(Space& home, & x0, BV x1);
    /// Constructor for posting
    ClauseTrue(Space& home, ViewArray<VX>& x, ViewArray<VY>& y);
    /// Post propagator for reflection
    static ExecStatus post(Space& home, VX x0, VY x1,
                           ViewArray<VX>& x, ViewArray<VY>& y);
    /// Constructor for cloning \a p
    ClauseTrue(Space& home, bool share, ClauseTrue<VX,VY>& p);
  public:
    /// Copy propagator during cloning
    virtual Actor* copy(Space& home, bool share);
    /// Cost function (defined as low binary)
    virtual PropCost cost(const Space& home, const ModEventDelta& med) const;
    /// Perform propagation
    virtual ExecStatus propagate(Space& home, const ModEventDelta& med);
    /// Post propagator \f$ \bigvee_{i=0}^{|x|-1} x_i \vee \bigvee_{i=0}^{|y|-1} y_i = 1\f$
    static  ExecStatus post(Space& home, ViewArray<VX>& x, ViewArray<VY>& y);
    /// Delete propagator and return its size
    virtual size_t dispose(Space& home);
  };


}}}

#include <gecode/int/bool/base.hpp>
#include <gecode/int/bool/eq.hpp>
#include <gecode/int/bool/lq.hpp>
#include <gecode/int/bool/or.hpp>
#include <gecode/int/bool/eqv.hpp>
#include <gecode/int/bool/clause.hpp>

#endif

// STATISTICS: int-prop

