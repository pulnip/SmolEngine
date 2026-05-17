#include "FramePacer.hpp"
#include "RHIDevice.hpp"

namespace Smol
{
    FramePacerRAII RHIDevice::CreateFramePacer(){
        return std::make_unique<FramePacer>(*this);
    }
}
