#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <string>
using namespace llvm;

namespace {
  struct CountStaticInstructions : public FunctionPass {
    static char ID;
    CountStaticInstructions() : FunctionPass(ID) {}
    bool runOnFunction(Function &F) override {
      std::map<std::string, int> countMap;
      for(BasicBlock &BB : F) {
        for(Instruction &I : BB) {
          countMap[I.getOpcodeName()]++;
        }
      }
      for (auto i = countMap.begin(); i != countMap.end(); ++i) {
        errs() << i -> first << "\t" <<  i->second << "\n";
      }
      return false;
    }
  };
}

char CountStaticInstructions::ID = 0;
static RegisterPass<CountStaticInstructions> X("cse231-csi", "cse231-csi", false, false);
