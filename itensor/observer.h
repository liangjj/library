//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_OBSERVER_H
#define __ITENSOR_OBSERVER_H

// virtual base class

class SVDWorker;

class Observer 
    {
    public:
    
    void virtual
    measure(int sw, int ha, int b, const SVDWorker& svd, Real energy,
            const OptSet& opts = Global::opts()) = 0;
    
    bool virtual
    checkDone(int sw, const SVDWorker& svd, Real energy, 
              const OptSet& opts = Global::opts()) = 0;

    virtual ~Observer() { }

    };

#endif 
