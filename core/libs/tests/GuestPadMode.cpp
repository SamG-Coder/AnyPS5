#include "prx/libScePad/include/Pad.hpp"
#include "prx/libScePad/include/PadState.hpp"
#include <cstdlib>

extern "C" int APS5_VABI scePadSetVibrationMode(int, int);
static void Require(bool value) { if (!value) std::abort(); }

int main() {
    Require(Pad::GetVibrationMode() == 2);
    Require(scePadSetVibrationMode(PAD_HANDLE, 1) == PAD_OK);
    Require(Pad::GetVibrationMode() == 1);
    Require(scePadSetVibrationMode(-1, 2) == PAD_ERROR_INVALID_HANDLE);
    Require(Pad::GetVibrationMode() == 1);
    for (int mode : {-1, 0, 3, 255}) {
        Require(scePadSetVibrationMode(PAD_HANDLE, mode) == PAD_ERROR_INVALID_ARG);
        Require(Pad::GetVibrationMode() == 1);
    }
    Require(scePadSetVibrationMode(PAD_HANDLE, 2) == PAD_OK);
    Require(scePadSetVibrationMode(PAD_HANDLE, 2) == PAD_OK);
    Require(Pad::GetVibrationMode() == 2);
}
