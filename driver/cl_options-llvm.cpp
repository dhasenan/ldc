//===-- cl_options-llvm.cpp -----------------------------------------------===//
//
//                         LDC � the LLVM D compiler
//
// This file is distributed under the BSD-style LDC license. See the LICENSE
// file for details.
//
//===----------------------------------------------------------------------===//

#include "driver/cl_options-llvm.h"

// Pull in command-line options and helper functions from special LLVM header
// shared by multiple LLVM tools.
#include "llvm/CodeGen/CommandFlags.h"

static cl::opt<bool>
    DisableRedZone("disable-red-zone", cl::ZeroOrMore,
                   cl::desc("Do not emit code that uses the red zone."));

// Now expose the helper functions (with static linkage) via external wrappers
// in the opts namespace, including some additional helper functions.
namespace opts {

std::string getArchStr() { return ::MArch; }

#if LDC_LLVM_VER >= 309
Optional<Reloc::Model> getRelocModel() { return ::getRelocModel(); }
#else
Reloc::Model getRelocModel() { return ::RelocModel; }
#endif

CodeModel::Model getCodeModel() { return ::CMModel; }

bool disableFPElim() { return ::DisableFPElim; }

bool disableRedZone() { return ::DisableRedZone; }

bool printTargetFeaturesHelp() {
  if (MCPU == "help")
    return true;
  return std::any_of(MAttrs.begin(), MAttrs.end(),
                     [](const std::string &a) { return a == "help"; });
}

TargetOptions InitTargetOptionsFromCodeGenFlags() {
  return ::InitTargetOptionsFromCodeGenFlags();
}

std::string getCPUStr() { return ::getCPUStr(); }
std::string getFeaturesStr() { return ::getFeaturesStr(); }
} // namespace opts

#if LDC_WITH_LLD && LDC_LLVM_VER >= 500
// LLD 5.0 uses the shared header too (for LTO) and exposes some wrappers in
// the lld namespace. Define them here to prevent the LLD object from being
// linked in with its conflicting command-line options.
namespace lld {
TargetOptions InitTargetOptionsFromCodeGenFlags() {
  return ::InitTargetOptionsFromCodeGenFlags();
}

CodeModel::Model GetCodeModelFromCMModel() { return CMModel; }
}
#endif // LDC_WITH_LLD && LDC_LLVM_VER >= 500
