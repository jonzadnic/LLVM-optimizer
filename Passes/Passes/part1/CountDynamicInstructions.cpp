#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/TypeBuilder.h"
#include <map>
#include "/lib231/lib231.cpp"

using namespace llvm;

namespace {

  struct CountDynamicInstructions : public FunctionPass {
    static char ID;
    CountDynamicInstructions() : FunctionPass(ID) {}
  bool runOnFunction(Function &F) override {
      Module* mod = F.getParent();
      Function *updateInstrInfo = cast<Function>(mod->getOrInsertFunction(
        "updateInstrInfo",
        Type::getVoidTy(F.getContext()),
        Type::getInt32Ty(F.getContext()),
        Type::getInt32PtrTy(F.getContext()),
        Type::getInt32PtrTy(F.getContext()
      )));

      Function *printOutInstrInfo = cast<Function>(mod->getOrInsertFunction(
        "printOutInstrInfo",
        Type::getVoidTy(F.getContext()),
        NULL));

      ArrayType* arrayTy = ArrayType::get(IntegerType::get(F.getContext(), 32), 65);
      unsigned* keys = new unsigned[65];
      for(int i = 0; i < 65; i++){
        keys[i] = i;
      }

      GlobalVariable* KEYS = new GlobalVariable(
        *mod,
        arrayTy,
        true,
        GlobalValue::InternalLinkage,
        ConstantDataArray::get(F.getContext(), *(new ArrayRef<uint32_t>(keys, 65))),
        "keys");

      for(auto &BB : F) {
          std::map<unsigned, unsigned> countMap;
          unsigned k = 0;
          for(Instruction &I : BB) {
              if(I.getOpcode()!=0)countMap[I.getOpcode()]++;
              if(k==BB.size()-1) {
                  unsigned* values = new unsigned[65];
                  for(int i = 0; i < 65; i++){
                    keys[i] = 0;
                  }
                  for (auto i = countMap.begin(); i != countMap.end(); ++i) {
                    values[i->first] = i->second;
                  }
                  GlobalVariable* VALUES = new GlobalVariable(
                    *mod,
                    arrayTy,
                    true,
                    GlobalValue::InternalLinkage,
                    ConstantDataArray::get(F.getContext(), *(new ArrayRef<uint32_t>(values, 65))),
                    "values");

                  IRBuilder<> Builder(&I);
                  Builder.SetInsertPoint(&BB, Builder.GetInsertPoint());
                  ConstantInt *num = ConstantInt::get(Type::getInt32Ty(F.getContext()), 65);
                  Value *args[] = {num,
                    Builder.CreatePointerCast(KEYS, Type::getInt32PtrTy(F.getContext())),
                    Builder.CreatePointerCast(VALUES, Type::getInt32PtrTy(F.getContext()))};
                  Builder.CreateCall(updateInstrInfo, args);
              }
              k++;
              if(I.getOpcode() == 1) {
                IRBuilder<> Builder(&I);
                Builder.SetInsertPoint(&BB, Builder.GetInsertPoint());
                Builder.CreateCall(printOutInstrInfo);
              }
          }

      }
      return false;
  }
};
}

char CountDynamicInstructions::ID = 0;
static RegisterPass<CountDynamicInstructions> X("cse231-cdi", "cse231-cdi", false, false);
