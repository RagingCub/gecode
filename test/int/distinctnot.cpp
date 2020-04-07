/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *     Mikael Lagerkvist <lagerkvist@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2005
 *     Mikael Lagerkvist, 2006
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

#include "test/int.hh"
#include <gecode/minimodel.hh>

namespace Test { namespace Int {

   /// %Tests for distinctnot constraints
   namespace Distinctnot {

     /**
      * \defgroup TaskTestIntDistinctnot Distinctnot constraints
      * \ingroup TaskTestInt
      */
     //@{
     /// Simple test for distinct constraint
     class Distinctnot : public Test {
     public:
       /// Create and register test
       Distinctnot(const Gecode::IntSet& d0, Gecode::IntPropLevel ipl,
                int n=6)
         : Test(std::string("aDistinctnot::")+str(ipl)+"::Sparse::"+str(n),n,d0,false,ipl) {}
       /// Create and register test
       Distinctnot(int min, int max, Gecode::IntPropLevel ipl)
         : Test(std::string("aDistinctnot::")+str(ipl)+"::Dense",6,min,max,false,ipl) {}
       /// Check whether \a x is solution
       virtual bool solution(const Assignment& x) const {
         for (int i=0; i<x.size(); i++)
           for (int j=i+1; j<x.size(); j++)
             if (x[i]==x[j])
               return true;
         return false;
       }
       /// Post constraint on \a x
       virtual void post(Gecode::Space& home, Gecode::IntVarArray& x) {
         Gecode::distinctnot(home, x, ipl);
       }
     };

     const int v[7] = {-1001,-1000,-10,0,10,1000,1001};
     Gecode::IntSet d(v,7);
     const int vl[6] = {Gecode::Int::Limits::min+0,
                        Gecode::Int::Limits::min+1,
                        Gecode::Int::Limits::min+2,
                        Gecode::Int::Limits::max-2,
                        Gecode::Int::Limits::max-1,
                        Gecode::Int::Limits::max-0};
     Gecode::IntSet dl(vl,6);

     Distinctnot dom_d(-3,3,Gecode::IPL_DOM);
     Distinctnot bnd_d(-3,3,Gecode::IPL_BND);
     Distinctnot val_d(-3,3,Gecode::IPL_VAL);
     Distinctnot dom_s(d,Gecode::IPL_DOM);
     Distinctnot bnd_s(d,Gecode::IPL_BND);
     Distinctnot val_s(d,Gecode::IPL_VAL);

     Distinctnot dom_l(dl,Gecode::IPL_DOM,5);
     Distinctnot bnd_l(dl,Gecode::IPL_BND,5);
     Distinctnot val_l(dl,Gecode::IPL_VAL,5);
     //@}

   }
}}

// STATISTICS: test-int
