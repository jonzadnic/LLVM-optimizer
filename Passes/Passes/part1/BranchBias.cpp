#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/TypeBuilder.h"


using namespace llvm;

namespace {
    struct BranchBias : public FunctionPass {
        static char ID;
        BranchBias() : FunctionPass(ID) {}

        bool runOnFunction(Function &F) override {
            LLVMContext &context = F.getContext();
            Module* mod = F.getParent();
            Function *printOutBranchInfo = cast<Function>(mod->getOrInsertFunction(
                    "printOutBranchInfo",
                    Type::getVoidTy(context)));
            Function *updateBranchInfo = cast<Function>(mod->getOrInsertFunction(
                    "updateBranchInfo",
                    Type::getVoidTy(context),
                    Type::getInt1Ty(context)));
            for(auto &BB : F) {
                for(auto &I : BB) {
                    if(I.getOpcode() == 2) {
                      if(I.getNumOperands() <= 1) continue;
                      IRBuilder<> Builder(&I);
                      Builder.SetInsertPoint(&BB, Builder.GetInsertPoint());
                      Builder.CreateCall(updateBranchInfo, I.getOperand(0));
                    }
                    else if(I.getOpcode() == 1) {
                      IRBuilder<> Builder(&I);
                      Builder.SetInsertPoint(&BB, Builder.GetInsertPoint());
                      Builder.CreateCall(printOutBranchInfo);
                    }
                }
            }
            return false;
        }
    };
}

char BranchBias::ID = 0;
static RegisterPass<BranchBias> X("cse231-bb", "cse231-bb", false, false);
