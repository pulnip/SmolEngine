#pragma once

#include <functional>
#include <type_traits>
#include <variant>
#include "Primitives.hpp"

namespace Smol
{
    // Defined at Lower layer
    struct UIContext;

    struct Checkbox{
        Str label;
        std::function<void(UIContext&, bool)> onChanged = [](UIContext&, bool){};
        bool v;

        void submit(UIContext&);
    };

    struct Box{
    private:
        class IWidget{
        public:
            virtual ~IWidget() = default;

            virtual void submit(UIContext& ctx) = 0;
            virtual RAII<IWidget> clone() const = 0;
        };

        template<typename T>
        class WidgetWrapper: public IWidget{
        private:
            T widget;

        public:
            WidgetWrapper(T widget)
                : widget(std::move(widget)){}
            ~WidgetWrapper() = default;

            void submit(UIContext& ctx) override{
                widget.submit(ctx);
            }
            RAII<IWidget> clone() const override{
                return std::make_unique<WidgetWrapper>(widget);
            }
        };

        RAII<IWidget> ptr;

    public:
        template<typename T>
        Box(T&& widget)
            : ptr(std::make_unique<WidgetWrapper<std::decay_t<T>>>(std::forward<T>(widget))){}
        ~Box() = default;
        Box(Box&&) = default;
        Box& operator=(Box&&) = default;
        Box(const Box& other)
            : ptr(other.ptr ? other.ptr->clone() : nullptr){}
        Box& operator=(const Box& other){
            ptr = other.ptr ? other.ptr->clone() : nullptr;
            return *this;
        }

        void submit(UIContext& ctx){
            ptr->submit(ctx);
        }
    };

    using Widget = std::variant<
        Checkbox,
        Box
    >;

    Widget Row(
        std::vector<Widget>&& children,
        double spacing = 0.0
    );

    Widget Column(
        std::vector<Widget>&& children,
        double spacing = 0.0
    );
}
