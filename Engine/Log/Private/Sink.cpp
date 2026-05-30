#include "Logger.hpp"
#include "Sink.hpp"

namespace Smol
{
    void AddSink(RAII<Sink> sink){
        static auto& logger = Logger::Get();

        logger.AddSink(std::move(sink));
    }
}
