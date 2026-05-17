#pragma once

#include <Foundation/NSAutoreleasePool.hpp>
#include "Semantics.hpp"

namespace Smol
{
    class AutoreleasePoolScope{
    private:
        NS::AutoreleasePool* pool = nullptr;

    public:
        AutoreleasePoolScope()
            : pool(NS::AutoreleasePool::alloc()->init()){}

        ~AutoreleasePoolScope(){
            if(pool != nullptr){
                pool->release();
                pool = nullptr;
            }
        }

        SMOL_DECLARE_PINNED(AutoreleasePoolScope)
    };
}
