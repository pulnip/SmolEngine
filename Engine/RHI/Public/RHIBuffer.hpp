#pragma once

#include <type_traits>
#include "Semantics.hpp"
#include "RHIDefinitions.hpp"

namespace Smol
{
    class RHIBuffer{
    public:
        SMOL_DECLARE_INTERFACE(RHIBuffer)

        template<typename T>
            requires (!std::is_pointer_v<T> && std::is_trivially_copyable_v<T>)
        void Upload(const T& data, u32 offset = 0){
            // type-safe helper
            Upload(&data, sizeof(T), offset);
        }

        // Notice! only valid for Metal and DX11
        template<typename T>
            requires (!std::is_pointer_v<T> && std::is_trivially_copyable_v<T>)
        void Download(T& data, u32 offset = 0){
            // type-safe helper
            Download(&data, sizeof(T), offset);
        }

        virtual u32 GetSize() const noexcept = 0;

        virtual RHIResourceState GetState() const = 0;
        virtual void SetState(RHIResourceState state) = 0;

    private:
        // direct use of void* is unsafe
        virtual void Upload(
            const void* data, u32 size,
            u32 offset = 0
        ) = 0;

        // direct use of void* is unsafe
        virtual void Download(
            void* data, u32 size,
            u32 offset = 0
        ) = 0;
    };
}
