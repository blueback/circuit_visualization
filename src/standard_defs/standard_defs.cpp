#include "standard_defs/standard_defs.hpp"

void *operator new(std::uint64_t size) { assert(0); }
void operator delete(void *ptr) throw() { assert(0); }
void *operator new[](std::uint64_t size) { assert(0); }
void operator delete[](void *ptr) throw() { assert(0); }

