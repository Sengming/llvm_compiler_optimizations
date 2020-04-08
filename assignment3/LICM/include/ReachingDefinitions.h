#ifndef __REACHING_DEFINITIONS_H__
#define __REACHING_DEFINITIONS_H__

#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include <KillGen.h>
#include <dataflow.h>

using namespace llvm;

/**
 * @brief Class to kill and gen Reaching Definitions list
 */
class KillGenRD : public KillGen<Value *> {
      public:
        std::bitset<MAX_BITS_SIZE>
        killEval(llvm::BasicBlock *BB, std::bitset<MAX_BITS_SIZE> &meet_res,
                 std::vector<Value *> &domainset) override;
        std::bitset<MAX_BITS_SIZE>
        genEval(llvm::BasicBlock *BB, std::bitset<MAX_BITS_SIZE> &meet_res,
                std::vector<Value *> &domainset) override;
};

/**
 * @brief Primary function pass to run Reaching Definitions pass.
 */
class ReachingDefsPass : public FunctionPass {
      protected:
        DenseMap<BasicBlock *, BBInOutBits *> *m_rdresults;

      public:
        static char ID;
        ReachingDefsPass();
        virtual bool runOnFunction(Function &F);

        virtual void getAnalysisUsage(AnalysisUsage &AU) const;

        DenseMap<BasicBlock *, BBInOutBits *> *getRDResults();
};
#endif
