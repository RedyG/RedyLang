#include "RedyModule.h"
#include "StructAST.h"
#include "TraitAST.h"
#pragma warning(disable:4146)
#include "llvm/IR/IRBuilder.h"
using namespace llvm;

void StructAST::GenLLVMType(LLVMContext& context, RedyModule& module) {
	if (Fields.size() == 0)
		return;

	std::vector<Type*> types;
	for (auto& field : Fields) {
		types.push_back(module.GetType(field.Variable.ReturnType, context)->LLVMType);
	}

	LLVMType = StructType::create(types, Name);
}

void TraitAST::GenLLVMType(LLVMContext& context, RedyModule& module) {
	// todo: dyn trait objects exemple: dyn Iterator
}