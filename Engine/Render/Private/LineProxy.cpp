#include "Assert.hpp"
#include "LineProxy.hpp"
#include "ShapeRenderer.hpp"

namespace Smol
{
    LineProxy::~LineProxy(){
        if(IsValid()){
            renderer->Unbind(handle);
        }
    }

    LineProxy::LineProxy(LineProxy&& other) noexcept
        : handle(other.handle), renderer(other.renderer)
    {
        other.handle = Handle::InvalidHandle();
        other.renderer = nullptr;
    }

    LineProxy& LineProxy::operator=(LineProxy&& other) noexcept{
        handle = other.handle;
        renderer = other.renderer;

        other.handle = Handle::InvalidHandle();
        other.renderer = nullptr;

        return *this;
    }

    Points& LineProxy::GetRef() noexcept{
        SMOL_ASSERT(IsValid());

        return renderer->Get(handle).points;
    }
}
