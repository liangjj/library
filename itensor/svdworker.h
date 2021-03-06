//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_SVDWORKER_H
#define __ITENSOR_SVDWORKER_H
#include "iqcombiner.h"
#include "iqtsparse.h"

#define Cout std::cout
#define Endl std::endl
#define Format boost::format

enum Direction { Fromright, Fromleft, Both, None };


//
// SVDWorker
//

class SVDWorker
    {
    public:

    //
    // Constructors
    //

    SVDWorker(const OptSet& opts = Global::opts());

    SVDWorker(int N, const OptSet& opts = Global::opts());

    SVDWorker(int N, Real cutoff, int minm, int maxm, 
              bool doRelCutoff, const LogNumber& refNorm);

    SVDWorker(std::istream& s) { read(s); }

    //
    // Site Canonical SVD 
    // (weights in both L and R, inverse weights in V)
    //

    template <class Tensor,class SparseT> 
    void 
    csvd(const Tensor& AA, Tensor& L, SparseT& V, Tensor& R)
        { 
        csvd<Tensor>(1,AA,L,V,R); 
        }

    template <class Tensor, class SparseT> 
    void 
    csvd(int b, const Tensor& AA, Tensor& L, SparseT& V, Tensor& R);

    //Object PH of type LocalOpT has to provide the deltaPhi method
    //to use the noise term feature (see localop.h)
    template <class Tensor, class SparseT, class LocalOpT>
    void 
    csvd(int b, const Tensor& AA, Tensor& L, SparseT& V, Tensor& R, 
         const LocalOpT& PH);


    //
    // Density Matrix Decomposition
    // (weights in B for dir == Fromleft, weights in A for dir == Fromright)
    //

    template <class Tensor> 
    void 
    denmatDecomp(const Tensor& AA, Tensor& A, Tensor& B, Direction dir)
        { 
        denmatDecomp<Tensor>(1,AA,A,B,dir); 
        }

    template <class Tensor> 
    void
    denmatDecomp(int b, const Tensor& AA, Tensor& A, Tensor& B, Direction dir);

    //Object PH of type LocalOpT has to provide the deltaRho method
    //to use the noise term feature (see localop.h)
    template <class Tensor, class LocalOpT>
    void
    denmatDecomp(int b, const Tensor& AA, Tensor& A, Tensor& B, Direction dir, 
                 const LocalOpT& PH);


    //
    // Singular Value Decomposition
    // (a.k.a. bond canonical SVD; weights in D)
    //

    template <class Tensor, class SparseT> 
    void 
    svd(const Tensor& AA, Tensor& U, SparseT& D, Tensor& V)
        { 
        svd<Tensor>(1,AA,U,D,V); 
        }

    template <class Tensor, class SparseT> 
    void 
    svd(int b, const Tensor& AA, Tensor& U, SparseT& D, Tensor& V);

    //Object PH of type LocalOpT has to provide the deltaPhi method
    //to use the noise term feature (see localop.h)
    template <class Tensor, class SparseT, class LocalOpT>
    void 
    svd(int b, Tensor AA, Tensor& U, SparseT& D, Tensor& V, 
        const LocalOpT& PH);

    //
    // Eigenvalue decomposition / diagonalization
    // Assumes input is a Hermitian tensor with indices
    // i,j,k,.... and i',j',k',...
    // (tensor must be conjugate symmetric under
    //  exchange primed and unprimed indices)
    // Result is unitary tensor U and diagonal sparse tensor D
    // such that M == U*D*primed(U)
    //

    template <class Tensor, class SparseT> 
    void 
    diagHermitian(const Tensor& M, Tensor& U, SparseT& D)
        { 
        diagHermitian<Tensor>(1,M,U,D); 
        }

    template <class Tensor, class SparseT> 
    void 
    diagHermitian(int b, const Tensor& M, Tensor& U, SparseT& D);

    //
    // Accessor Methods
    //

    int 
    N() const { return N_; }

    Real 
    cutoff() const { return cutoff_; }
    void 
    cutoff(Real val) { cutoff_ = val; }

    Real 
    truncerr(int b = 1) const { return truncerr_.at(b); }

    int 
    minm() const { return minm_; }
    void 
    minm(int val) { minm_ = val; }

    int 
    maxm() const { return maxm_; }
    void 
    maxm(int val) { maxm_ = val; }

    //Perform the SVD, but do not truncate.
    //Try to keep the original bond dimension 
    //as determined by the shared indices of 
    //the tensors holding the factorized
    //pieces of the original tensor.
    bool 
    useOrigM() const { return use_orig_m_; }
    void 
    useOrigM(bool val) { use_orig_m_ = val; }

    //Print detailed information about the
    //eigenvalues computed during the SVD
    bool 
    showeigs() const { return showeigs_; }
    void 
    showeigs(bool val) { showeigs_ = val; }

    // If doRelCutoff_ is false,
    // refNorm_ defines an overall scale factored
    // out of the denmat before truncating.
    //
    // If doRelCutoff_ is true, refNorm_
    // is determined automatically.
    //
    // (Default is false.)
    //
    bool 
    doRelCutoff() const { return doRelCutoff_; }
    void 
    doRelCutoff(bool val) { doRelCutoff_ = val; }

    // If absoluteCutoff_ == true, do a strict limit
    // on the eigenvalues of the density matrix, 
    // not trying for a truncation error
    bool 
    absoluteCutoff() const { return absoluteCutoff_; }
    void 
    absoluteCutoff(bool val) { absoluteCutoff_ = val; }

    LogNumber 
    refNorm() const { return refNorm_; }
    void 
    refNorm(const LogNumber& val) 
        { 
        if(val.sign() == 0) Error("zero refNorm");
        refNorm_ = val; 
        }

    const Vector& 
    eigsKept(int b = 1) const 
        { return eigsKept_.at(b); }
    int
    numEigsKept(int b = 1) const 
        { return eigsKept_.at(b).Length(); }

    int 
    maxEigsKept() const;

    Real 
    maxTruncerr() const;

    Real
    noise() const { return noise_; }
    void
    noise(Real val) { noise_ = val; }

    //
    // Other Methods
    //

    void 
    read(std::istream& s);
    void 
    write(std::ostream& s) const;

    private:

    Real
    truncate(Vector& eigs);
    Real
    truncate(std::vector<Real>& eigs, int& m, Real& docut);

    Real 
    diag_hermitian(ITensor rho, ITensor& U, ITSparse& D, int b,
                   const OptSet& opts = Global::opts());

    Real 
    diag_hermitian(IQTensor rho, IQTensor& U, IQTSparse& D, int b,
                   const OptSet& opts = Global::opts());


    void 
    svdRank2(ITensor A, const Index& ui, const Index& vi,
             ITensor& U, ITSparse& D, ITensor& V, int b = 1);

    void 
    svdRank2(IQTensor A, const IQIndex& uI, const IQIndex& vI,
             IQTensor& U, IQTSparse& D, IQTensor& V, int b = 1);


    void
    initOpts(const OptSet& opts);

    ITensor 
    pseudoInverse(const ITensor& C, Real cutoff = 0);

    IQTensor 
    pseudoInverse(const IQTensor& C, Real cutoff = 0);

    /////////////////
    //
    // Data Members
    //

    int N_;
    std::vector<Real> truncerr_;
    Real cutoff_;
    int minm_;
    int maxm_;
    bool use_orig_m_,
         showeigs_,
         doRelCutoff_,
         absoluteCutoff_,
         truncate_;
    LogNumber refNorm_;
    std::vector<Vector> eigsKept_;
    Real noise_;

    //
    /////////////////

    //Function object which applies the mapping
    // f(x) = (x < cut ? 0 : 1/x);
    class PseudoInverter
        {
        public:
            PseudoInverter(Real cut = MIN_CUT)
                :
                cut_(cut)
                { }

            Real
            operator()(Real val) const
                {
                if(fabs(val) > cut_)
                    return 1./val;
                else
                    return 0;
                }
        private:
            Real cut_;
        };

    }; //class SVDWorker

//
// Convenience wrappers of SVDWorker methods
// svd, csvd, and denmatDecomp.
//

template <class Tensor,class SparseT>
void
svd(const Tensor& T, Tensor& U, SparseT& D, Tensor& V,
    const OptSet& opts = Global::opts())
    {
    SVDWorker W(opts);
    W.svd(T,U,D,V);
    }

template <class Tensor,class SparseT>
void
csvd(const Tensor& T, Tensor& L, SparseT& V, Tensor& R,
     const OptSet& opts = Global::opts())
    {
    SVDWorker W(opts);
    W.svd(T,L,V,R);
    }

template <class Tensor>
void
denmatDecomp(const Tensor& T, Tensor& A, Tensor& B, Direction dir,
             const OptSet& opts = Global::opts())
    {
    SVDWorker W(opts);
    W.denmatDecomp(T,A,B,dir);
    }

template <class Tensor,class SparseT>
void
diagHermitian(const Tensor& T, Tensor& U, SparseT& D,
            const OptSet& opts = Global::opts())
    {
    SVDWorker W(opts);
    W.diagHermitian(T,U,D);
    }

//
// SVDWorker template methods
//

template<class Tensor, class SparseT, class LocalOpT>
void SVDWorker::
csvd(int b, const Tensor& AA, Tensor& L, SparseT& V, Tensor& R, 
     const LocalOpT& PH)
    {
    typedef typename Tensor::IndexT 
    IndexT;
    typedef typename Tensor::CombinerT 
    CombinerT;

    Tensor UU(L),VV(R);
    SparseT D(V);
    svd(b,AA,UU,D,VV,PH);

    L = UU*D;
    R = D*VV;

    V = conj(D);
    V.pseudoInvert(0);

    } //void SVDWorker::csvd

template<class Tensor, class SparseT, class LocalOpT>
void SVDWorker::
svd(int b, Tensor AA, Tensor& U, SparseT& D, Tensor& V, 
    const LocalOpT& PH)
    {
    typedef typename Tensor::IndexT 
    IndexT;
    typedef typename Tensor::CombinerT 
    CombinerT;
    
    if(AA.vecSize() == 0) 
        throw ResultIsZero("denmatDecomp: AA.vecSize == 0");

    if(noise_ > 0)
        Error("Noise term not implemented for svd");

    //if(noise_ > 0 && !PH.isNull())
    //    {
    //    //Add in noise term
    //    Real orig_norm = AA.norm();
    //    AA += noise_*PH.deltaPhi(AA);
    //    AA *= orig_norm/AA.norm();
    //    }

    //Combiners which transform AA
    //into a rank 2 tensor
    CombinerT Ucomb, Vcomb;
    Ucomb.doCondense(true);
    Vcomb.doCondense(true);

    //Divide up indices based on U
    //If U is null, use V instead
    const Tensor &L = (U.isNull() ? V : U);
    CombinerT &Lcomb = (U.isNull() ? Vcomb : Ucomb),
              &Rcomb = (U.isNull() ? Ucomb : Vcomb);
    Foreach(const IndexT& I, AA.indices())
        { 
        if(I.type() == ReIm) continue;

        if(hasindex(L,I))
            Lcomb.addleft(I);
        else
            Rcomb.addleft(I);
        }

    AA = Ucomb * AA * Vcomb;

    const Real saved_cutoff = cutoff_; 
    const int saved_minm = minm_,
              saved_maxm = maxm_; 
    if(use_orig_m_)
        {
        //Try to determine current m,
        //then set minm_ and maxm_ to this.
        cutoff_ = -1;
        if(D.r() == 0)
            {
            try {
                IndexT mid = commonIndex(U,V,Link);
                minm_ = mid.m();
                maxm_ = mid.m();
                }
            catch(const ITError& e)
                {
                minm_ = 1;
                maxm_ = 1;
                }
            }
        else
            {
            minm_ = D.index(1).m();
            maxm_ = D.index(1).m();
            }
        //if(Global::debug1())
        //    {
        //    Cout << "Increasing maxm by 10" << Endl;
        //    maxm_ += 10;
        //    }
        }

    svdRank2(AA,Ucomb.right(),Vcomb.right(),U,D,V,b);

    cutoff_ = saved_cutoff; 
    minm_ = saved_minm; 
    maxm_ = saved_maxm; 

    U = conj(Ucomb) * U;
    V = V * conj(Vcomb);

    } // void SVDWorker::svd

template<class Tensor, class LocalOpT>
void SVDWorker::
denmatDecomp(int b, const Tensor& AA, Tensor& A, Tensor& B, Direction dir, 
             const LocalOpT& PH)
    {
    typedef typename Tensor::IndexT 
    IndexT;
    typedef typename Tensor::CombinerT 
    CombinerT;
    typedef typename Tensor::SparseT 
    SparseT;

    if(AA.vecSize() == 0) 
        throw ResultIsZero("denmatDecomp: AA.vecSize == 0");

    IndexT mid; 
    try {
        mid = commonIndex(A,B,Link);
        }
    catch(const ITError& e)
        {
        //continue, leaving mid default-initialized
        }

    //If dir==None, put the O.C. on the side
    //that keeps mid's arrow the same
    if(dir == None)
        {
        //Cout << Format("Arrow before = %s")%(mid.dir() == Out ? "Out" : "In") << Endl;
        dir = (mid.dir() == Out ? Fromright : Fromleft);
        }

    Tensor& to_orth = (dir==Fromleft ? A : B);
    Tensor& newoc   = (dir==Fromleft ? B : A);

    CombinerT comb;

    Foreach(const IndexT& I, to_orth.indices())
        { 
        if(!(hasindex(newoc,I) || I == Tensor::ReImIndex() ))
            comb.addleft(I);
        }

    //Apply combiner
    comb.doCondense(true);
    comb.init(mid.isNull() ? "mid" : mid.rawname());

    Tensor AAc; 
    comb.product(AA,AAc);

    //Form density matrix
    Tensor AAcc = conj(AAc); 
    AAcc.prime(comb.right()); 

    Tensor rho = AAc*AAcc; 

    //Add noise term if requested
    if(noise_ > 0 && !PH.isNull())
        {
        rho += noise_*PH.deltaRho(AA,comb,dir);
        rho *= 1./trace(realPart(rho));
        }

    const Real saved_cutoff = cutoff_; 
    const int saved_minm = minm_,
              saved_maxm = maxm_; 
    if(use_orig_m_)
        {
        cutoff_ = -1;
        minm_ = mid.m();
        maxm_ = mid.m();
        }

    Tensor U;
    SparseT D;
    truncerr_.at(b) = diag_hermitian(rho,U,D,b,Opt("TraceReIm",true));

    cutoff_ = saved_cutoff; 
    minm_ = saved_minm; 
    maxm_ = saved_maxm; 

    comb.conj();
    comb.product(conj(U),to_orth);
    newoc = U * AAc;

    } //void SVDWorker::denmatDecomp

template<class Tensor, class SparseT>
void SVDWorker::
diagHermitian(int b, const Tensor& M, Tensor& U, SparseT& D)
    {
    typedef typename Tensor::IndexT 
    IndexT;
    typedef typename Tensor::CombinerT 
    CombinerT;

    if(M.vecSize() == 0) 
        throw ResultIsZero("denmatDecomp: AA.vecSize == 0");

    CombinerT comb;
    Foreach(const IndexT& I, M.indices())
        { 
        if(I.type() == ReIm) continue;
        if(I.primeLevel() == 0)
            {
            comb.addleft(I);
            }
        }

    //Apply combiner
    comb.doCondense(true);
    comb.init("d");

    Tensor Mc; 
    comb.product(M,Mc);

    CombinerT combP(comb);
    combP.prime();
    combP.conj();

    try {
        Mc = combP * Mc;
        }
    catch(const ITError& e)
        {
        Cout << "Diagonalize expects opposite arrow directions for primed and unprimed indices." << Endl;
        throw e;
        }

    const bool truncate_setting = truncate_;
    truncate_ = false;
    diag_hermitian(Mc,U,D,b);
    truncate_ = truncate_setting;

    U = comb * U;

    } //void SVDWorker::diagHermitian

#undef Cout
#undef Format
#undef Endl


#endif
