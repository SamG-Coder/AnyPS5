#pragma once
#include "SceTypes.hpp"
namespace GuestThreadClocks {
void Release(int id);
int Read(int id, KernelTimespec* output);
int Resolution(int id, KernelTimespec* output);
}
