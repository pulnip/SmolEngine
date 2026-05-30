#include "Log.hpp"
#include "Logger.hpp"

namespace Smol
{
    void detail::Log(LogMessage&& msg){
        static auto& logger = Logger::Get();

        logger.Log(std::move(msg));
    }
}
