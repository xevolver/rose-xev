#ifndef ROSE_Partitioner2_ModulesX86_H
#define ROSE_Partitioner2_ModulesX86_H

#include <Partitioner2/Modules.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace ModulesX86 {

/** Matches an x86 function prologue.
 *
 *  The standard x86 function prologue is:
 *
 * @code
 *  push ebp
 *  mov ebp, esp
 * @endcode
 *
 *  The width of ebp and esp must match the word size for the architecture (i.e., they must be EBP and ESP for the i386 family,
 *  and RBP, RSP for the amd64 family). */
class MatchStandardPrologue: public FunctionPrologueMatcher {
protected:
    Function::Ptr function_;
public:
    static Ptr instance() { return Ptr(new MatchStandardPrologue); } /**< Allocating constructor. */
    virtual std::vector<Function::Ptr> functions() const ROSE_OVERRIDE { return std::vector<Function::Ptr>(1, function_); }
    virtual bool match(const Partitioner &partitioner, rose_addr_t anchor) ROSE_OVERRIDE;
};

/** Matches an x86 function prologue with hot patch.
 *
 *  A hot-patch prologue is a MOV EDI, EDI instruction followed by a standard prologue. These are generated by Microsoft
 *  compilers.
 *
 *  @todo: FIXME[Robb P. Matzke 2014-08-24]: this function should also check for and attach the bytes prior to the MOV EDI,
 *  EDI. We should also have a version that matches after a patch is activated in case we're disassembling the memory of a
 *  process. */
class MatchHotPatchPrologue: public MatchStandardPrologue {
public:
    static Ptr instance() { return Ptr(new MatchHotPatchPrologue); } /**< Allocating constructor. */
    virtual std::vector<Function::Ptr> functions() const ROSE_OVERRIDE { return std::vector<Function::Ptr>(1, function_); }
    virtual bool match(const Partitioner &partitioner, rose_addr_t anchor) ROSE_OVERRIDE;
};

/** Matches an x86 <code>MOV EDI,EDI; PUSH ESI</code> function prologe. */
class MatchAbbreviatedPrologue: public FunctionPrologueMatcher {
protected:
    Function::Ptr function_;
public:
    static Ptr instance() { return Ptr(new MatchAbbreviatedPrologue); }
    virtual std::vector<Function::Ptr> functions() const ROSE_OVERRIDE { return std::vector<Function::Ptr>(1, function_); }
    virtual bool match(const Partitioner &partitioner, rose_addr_t anchor) ROSE_OVERRIDE;
};

/** Matches an x86 "ENTER xxx, 0" prologue. */
class MatchEnterPrologue: public FunctionPrologueMatcher {
protected:
    Function::Ptr function_;
public:
    static Ptr instance() { return Ptr(new MatchEnterPrologue); } /**< Allocating constructor. */
    virtual std::vector<Function::Ptr> functions() const ROSE_OVERRIDE { return std::vector<Function::Ptr>(1, function_); }
    virtual bool match(const Partitioner &partitioner, rose_addr_t anchor) ROSE_OVERRIDE;
};

/** Match thunk.
 *
 *  Matches any thunk of the form matched by @ref isThunk.  This callback is invoked at addresses that are not part of the
 *  partitioner's CFG and will only match instructions not in the CFG. */
class MatchThunk: public FunctionPrologueMatcher {
protected:
    std::vector<Function::Ptr> functions_;
public:
    static Ptr instance() { return Ptr(new MatchThunk); } /**< Allocating constructor. */
    virtual std::vector<Function::Ptr> functions() const ROSE_OVERRIDE { return functions_; }
    virtual bool match(const Partitioner&, rose_addr_t anchor) ROSE_OVERRIDE;
};

/** Match RET followed by PUSH with intervening no-op padding. */
class MatchRetPadPush: public FunctionPrologueMatcher {
protected:
    Function::Ptr function_;
public:
    static Ptr instance() { return Ptr(new MatchRetPadPush); } /**< Allocating constructor. */
    virtual std::vector<Function::Ptr> functions() const ROSE_OVERRIDE { return std::vector<Function::Ptr>(1, function_); }
    virtual bool match(const Partitioner &partitioner, rose_addr_t anchor) ROSE_OVERRIDE;
};

/** Basic block callback to detect function returns.
 *
 *  The architecture agnostic isFunctionReturn test for basic blocks does not detect x86 "RET N" (N!=0) instructions as
 *  returning from a function because such instructions have side effects that apply after the return-to address is popped from
 *  the stack.  Therefore this basic block callback looks for such instructions and sets the isFunctionReturn property for the
 *  basic block. */
class FunctionReturnDetector: public BasicBlockCallback {
public:
    static Ptr instance() { return Ptr(new FunctionReturnDetector); } /**< Allocating constructor. */
    virtual bool operator()(bool chain, const Args&) ROSE_OVERRIDE;
};

/** Basic block callback to detect "switch" statements.
 *
 *  Examines the instructions of a basic block to determine if they are from a C "switch"-like statement and attempts to find
 *  the "case" labels, adding them as successors to this basic block. */
class SwitchSuccessors: public BasicBlockCallback {
public:
    static Ptr instance() { return Ptr(new SwitchSuccessors); } /**< Allocating constructor. */
    virtual bool operator()(bool chain, const Args&) ROSE_OVERRIDE;
};

/** Determines whether an instruction sequence begins with a thunk.  There are a number of these functions:
 *
 *  @li @ref isJmpImmThunk looks for "JMP address"
 *  @li @ref isJmpMemThunk looks for "JMP [address]"
 *  @li @ref isLeaJmpThunk looks for "LEA ECX, [EBP+const]; JMP address"
 *  @li @ref isMovJmpThunk looks for "MOV reg, [address]; JMP reg"
 *
 *  The @ref isThunk function tries to match any of the above patterns.  The return value is the number of instructions that
 *  were matched.
 *
 * @{ */
size_t isJmpMemThunk(const Partitioner&, const std::vector<SgAsmInstruction*>&);
size_t isLeaJmpThunk(const Partitioner&, const std::vector<SgAsmInstruction*>&);
size_t isMovJmpThunk(const Partitioner&, const std::vector<SgAsmInstruction*>&);
size_t isJmpImmThunk(const Partitioner&, const std::vector<SgAsmInstruction*>&);
size_t isThunk(const Partitioner&, const std::vector<SgAsmInstruction*>&);
/** @} */

/** Matches "ENTER x, 0" */
bool matchEnterAnyZero(const Partitioner&, SgAsmX86Instruction*);

/** Matches "JMP constant".
 *
 *  Returns the constant if matched, nothing otherwise. */
Sawyer::Optional<rose_addr_t> matchJmpConst(const Partitioner&, SgAsmX86Instruction*);

/** Matches "LEA ECX, [EBP + constant]" or variant. */
bool matchLeaCxMemBpConst(const Partitioner&, SgAsmX86Instruction*);

/** Matches "JMP [address]" or variant. */
bool matchJmpMem(const Partitioner&, SgAsmX86Instruction*);

/** Matches "MOV EBP, ESP" or variant. */
bool matchMovBpSp(const Partitioner&, SgAsmX86Instruction*);

/** Matches "MOV EDI, EDI" or variant. */
bool matchMovDiDi(const Partitioner&, SgAsmX86Instruction*);

/** Matches "PUSH EBP" or variant. */
bool matchPushBp(const Partitioner&, SgAsmX86Instruction*);

/** Matches "PUSH SI" or variant. */
bool matchPushSi(const Partitioner&, SgAsmX86Instruction*);

/** Returns number of leading instructions that form a valid thunk. */
size_t isThunk(const std::vector<SgAsmInstruction*> &insns);

/** Split thunks off from start of functions.
 *
 *  Splits as many thunks as possible off the front of all functions currently attached to the partitioner's CFG. */
void splitThunkFunctions(Partitioner&);

/** Reads a table of code addresses.
 *
 *  Reads a table that starts at the lower limit of @p tableLimits and does not extend past the upper limit.  Each entry in the
 *  table is an instruction address of @p tableEntrySize bytes and the entry must exist in read-only memory.  The address
 *  stored in the entry must be within the @p targetLimits interval and must be an address that is mapped with execute
 *  permission.  As many entries as possible are read into the return vector.  Upon return, the @p tableLimits is adjusted to
 *  indicate the actual location of the table. */
std::vector<rose_addr_t> scanCodeAddressTable(const Partitioner&, AddressInterval &tableLimits /*in,out*/,
                                              const AddressInterval &targetLimits, size_t tableEntrySize);

/** Try to match a base+offset expression.
 *
 *  Matches expressions like:
 *
 * @li base + register
 * @li base + register * size
 * @li [ base + register ]
 * @li [ base + register * size ]
 *
 * Returns the numeric value of @c base or nothing if the expression is not a recognized form. */
Sawyer::Optional<rose_addr_t> findTableBase(SgAsmExpression*);


} // namespace
} // namespace
} // namespace
} // namespace

#endif
