// ECE 5984 S20 Assignment 3: licm.cpp
// Group: Ashwin & SengMing

////////////////////////////////////////////////////////////////////////////////
// llvm specific
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
// Framework and ours
#include <DominatorPass.h>
#include <IntersectionMeet.h>
#include <KillGen.h>
#include <ReachingDefinitions.h>
#include <dataflow.h>
#include <vector>
namespace llvm {
enum iv_status { INVARIENT, VARIENT };

/**
 * @brief Primary function pass to run LICM (Loop Invariant Code Motion) pass.
 * Prerequisite is dominators pass and RD pass must have been run. This is just
 * currently for testing, we will have to change this to a loop pass later
 */
class LICM : public LoopPass {
      public:
        static char ID;
        llvm::DenseMap<Value *, iv_status> Loop_Invarients;
        LoopInfo *LI;
        DenseMap<BasicBlock *, BBInOutBits *> *reachingDefs;
        DenseMap<BasicBlock *, BBInOutBits *> *DomResult;
        LICM() : LoopPass(ID) {}
        bool isInvarient(Instruction *I, std::vector<Value *> l_ins) {
                bool result = isSafeToSpeculativelyExecute(I) &&
                              !I->mayReadFromMemory() &&
                              !isa<LandingPadInst>(I);
                if (result && Loop_Invarients[I] == VARIENT) {
                        for (int i = 0; i < I->getNumOperands(); ++i) {
                                auto it = std::find(l_ins.begin(), l_ins.end(),
                                                    I->getOperand(i));
                                if (it != l_ins.end()) {
                                        if (Loop_Invarients[*it] == VARIENT) {
                                                result = false;
                                        }
                                }
                        }
                        if (result) {
                                Loop_Invarients[I] = INVARIENT;
                        }
                }
                return result;
        }
        virtual bool runOnLoop(Loop *L, LPPassManager &LPM) {
                reachingDefs = getAnalysis<ReachingDefsPass>().getRDResults();
                DomResult = getAnalysis<DominatorsPass>().getDomResults();
                std::vector<Value *> loop_vals, invarient_vals;
                size_t old_size = 0;
                BasicBlock *preHeader = L->getLoopPreheader();
                for (int i = 0; i < 100; i++)
                        outs() << "=";
                outs() << "\nLoop analysis pass :\n ";
                outs() << "The Loop pre header is " << *(L->getLoopPreheader())
                       << "\n";
                outs() << "Basic Blocks in the loop \n\n";

                if (L->getLoopPreheader() == NULL) {
                        outs() << "Loop pre header not present\n";
                } else {
                        outs() << "Eligible for loop transform\n";
                }

                outs() << "Binary Instruction list in loop\n";
                for (Loop::block_iterator i = L->block_begin(),
                                          e = L->block_end();
                     i != e; ++i) {
                        BasicBlock *BB = *i;
                        for (BasicBlock::iterator i = BB->begin(),
                                                  e = BB->end();
                             i != e; ++i) {
                                if (isa<BinaryOperator>(*i) ||
                                    isa<PHINode>(*i)) {
                                        Value *V = &*i;
                                        loop_vals.push_back(V);
                                        Loop_Invarients[V] = VARIENT;
                                        outs() << "\t" << *V << "\n";
                                }
                        }
                }

                do {
                        outs()
                            << "Old size : " << old_size
                            << " prev iter : " << invarient_vals.size() << "\n";
                        old_size = invarient_vals.size();
                        if (L->getLoopPreheader() != NULL) {
                                for (Loop::block_iterator i = L->block_begin(),
                                                          e = L->block_end();
                                     i != e; ++i) {

                                        BasicBlock *BB = *i;
                                        //        outs() << *BB << "\n\n";
                                        for (BasicBlock::iterator
                                                 i = BB->begin(),
                                                 e = BB->end();
                                             i != e; ++i) {

                                                Instruction *I = &*i;
                                                if (isa<BinaryOperator>(I) ||
                                                    isa<PHINode>(I)) {
                                                        bool res = isInvarient(
                                                            I, loop_vals);

                                                        outs() << *I;
                                                        if (res) {
                                                                auto it = std::find(
                                                                    invarient_vals
                                                                        .begin(),
                                                                    invarient_vals
                                                                        .end(),
                                                                    I);
                                                                if (it ==
                                                                    invarient_vals
                                                                        .end()) {
                                                                        invarient_vals
                                                                            .push_back(
                                                                                I);
                                                                }
                                                                outs()
                                                                    << " is "
                                                                       "invarie"
                                                                       "nt\n";
                                                        } else {
                                                                outs()
                                                                    << " not "
                                                                       "invarie"
                                                                       "nt\n";
                                                        }
                                                }
                                        }
                                }
                        }
                } while (old_size != invarient_vals.size());
                for (auto it : invarient_vals) {

                        outs() << *it << "\n";
                        //((Instruction *)it)->removeFromParent();
                        //   preHeader->moveBefore(
                        //     (Instruction *)(it));
                        ((Instruction *)it)
                            ->moveBefore(preHeader->getTerminator());
                }
                // Compute loop invariant statements - done
                // getLoopInvariantStatements();     - done
                // checkConditionForCodeMotion();
                return false;
        }
        virtual void getAnalysisUsage(AnalysisUsage &AU) const {
                AU.setPreservesCFG();
                AU.addRequired<ReachingDefsPass>();
                AU.addRequired<DominatorsPass>();
                AU.addRequired<LoopInfoWrapperPass>();
        }
};

char LICM::ID = 1;
RegisterPass<LICM> licm("licm_pass", "ECE 5984 LICM Pass");
} // namespace llvm
