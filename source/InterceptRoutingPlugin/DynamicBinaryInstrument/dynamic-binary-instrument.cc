#include "dobby_internal.h"

#include "PlatformInterface/ExecMemory/CodePatchTool.h"
#include "ExecMemory/ExecutableMemoryArena.h"

#include "ClosureTrampolineBridge/AssemblyClosureTrampoline.h"

#include "InterceptRoutingPlugin/DynamicBinaryInstrument/dynamic-binary-instrument.h"
#include "InterceptRoutingPlugin/DynamicBinaryInstrument/intercept_routing_handler.h"

void DynamicBinaryInstrumentRouting::Dispatch() {
  Prepare();
  BuildDynamicBinaryInstrumentRouting();
}

// Add dbi_call handler before running the origin instructions
void DynamicBinaryInstrumentRouting::BuildDynamicBinaryInstrumentRouting() {
  // create closure trampoline jump to prologue_routing_dispath with the `entry_` data
  ClosureTrampolineEntry *closure_trampoline;
  // forward trampoline
  closure_trampoline = ClosureTrampoline::CreateClosureTrampoline(entry_, (void *)instrument_routing_dispatch);
  DLOG("create dynamic binary instrumentation call closure trampoline to 'prologue_dispatch_bridge' %p\n",
       closure_trampoline_entry->address);

  // hijack trampoline
  this->trampoline = (CodeBufferBase *)GenTrampoline(entry_->target_address, GetTrampolineTarget());
  DLOG("create 'hijack trampoline' %p\n", this->trampoline);

  this->prologue_dispatch_bridge = closure_trampoline_entry->address;
}

void *DynamicBinaryInstrumentRouting::GetTrampolineTarget() {
  return this->prologue_dispatch_bridge;
}