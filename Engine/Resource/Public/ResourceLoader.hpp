#pragma once

#include <vector>
#include "GenericHandle.hpp"
#include "Resource.hpp"
#include "Semantics.hpp"

namespace Smol
{
    template<Resource T>
    class ResourceLoader{
    public:
        using Request = typename T::Request;
        using Handle = GenericHandle<T>;

        struct Completion{
            T data;
            Handle handle;
        };

    public:
        SMOL_DECLARE_INTERFACE_NOEXCEPT(ResourceLoader)

        virtual void Submit(const Request&, Handle) = 0;
        virtual void Poll(std::vector<Completion>&) = 0;
    };
}
