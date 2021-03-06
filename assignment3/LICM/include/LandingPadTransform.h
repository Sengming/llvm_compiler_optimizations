#ifndef __LANDINGPADTRANSFORM_H___
#define __LANDINGPADTRANSFORM_H___

#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/Analysis/LoopPass.h>

namespace llvm {

/**
 * @brief Primary function pass to run Reaching Definitions pass.
 */
class LandingPadTransform : public LoopPass {
      protected:
        BasicBlock *removePhiDependencies(BasicBlock *newtest,
                                          BasicBlock *header);
        void
        updatePhiNotInLoop(Loop &loop,
                           SmallVector<Instruction *, 10> &previousPhiUsers,
                           PHINode *from, PHINode *to);
        void unifyPhiAtExit(BasicBlock *newtest, BasicBlock *unifiedExit,
                            BasicBlock *loopexit, BasicBlock *header,
                            BasicBlock *lastBody, Loop *L);

      public:
        static char ID;
        LandingPadTransform();
        virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override;
        virtual void getAnalysisUsage(AnalysisUsage &AU) const override;
};
} // namespace llvm
#endif
