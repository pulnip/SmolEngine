#include "AppConfig.hpp"
#include "AppMainLoop.hpp"
#include "CharacterController.hpp"
#include "ColliderComponent.hpp"
#include "CommandListPool.hpp"
#include "ImmediateResourceLoader.hpp"
#include "InputComponent.hpp"
#include "LineRenderer.hpp"
#include "LogLocal.hpp"
#include "MoveComponent.hpp"
#include "OS.hpp"
#include "Pawn.hpp"
#include "Primitives.hpp"
#include "RHICommandList.hpp"
#include "RHIDefinitions.hpp"
#include "RHIDevice.hpp"
#include "RHISwapchain.hpp"
#include "RHITexture.hpp"
#include "SpriteComponent.hpp"
#include "SpriteAnimComponent.hpp"
#include "TomlLoader.hpp"
#include "Window.hpp"

#include "CameraConfig.hpp"
#include "RHIBuffer.hpp"
#include "RHIPipelineState.hpp"
#include "RHISampler.hpp"

namespace{
    template<std::derived_from<Smol::Component> T>
    T* createComponent(
        Smol::Actor& actor,
        const Smol::DOM::Value& dom
    ){
        auto component = actor.AddComponent<T>();

        ApplyProperties(component, dom);
        return component;
    }

    void attachComponent(
        Smol::Actor& actor,
        const Smol::DOM::Value& dom,
        const std::filesystem::path& contentRoot
    ){
        using namespace Smol;

        auto type = dom.get<Str>("type");
        if(!type.has_value()) [[unlikely]]{
            LOG_ERROR("no \"type\" key in [[actors.component]]");
            return;
        }

        if(*type == "InputComponent"){
            createComponent<InputComponent>(actor, dom);
        }
        else if(*type == "LineRenderer"){
            createComponent<LineRenderer>(actor, dom);
        }
        else if(*type == "MoveComponent"){
            createComponent<MoveComponent>(actor, dom);
        }
        else if(*type == "SpriteAnimComponent"){
            auto component = createComponent<SpriteAnimComponent>(actor, dom);
            auto sprite = dom.get<Str>("sprite");
            if(!sprite.has_value()){
                LOG_ERROR("no \"sprite\" in [[actors.component]] (type=SpriteAnimComponent)");
            }
            else{
                component->OnAttach(*sprite);
            }
        }
        else if(*type == "SpriteComponent"){
            auto component = createComponent<SpriteComponent>(actor, dom);
            auto sprite = dom.get<Str>("sprite");
            if(!sprite.has_value()){
                LOG_ERROR("no \"sprite\" in [[actors.component]] (type=SpriteComponent)");
            }
            else{
                component->OnAttach(*sprite);
            }
        }
        else if(*type == "ColliderComponent"){
            auto component = createComponent<ColliderComponent>(actor, dom);
            component->OnAttach();
        }
        else{
            // user-defined Component
            auto component = actor.AddComponent(*type);

            ApplyProperties(component, dom);
        }
    }

    template<std::derived_from<Smol::Actor> T>
    T* createActor(
        Smol::World& world,
        const Smol::DOM::Value& dom,
        const Smol::Str& name
    ){
        auto actor = world.SpawnActor<T>(name);

        ApplyProperties(actor, dom);
        return actor;
    }

    void applyPossessedBy(
        Smol::Pawn& pawn,
        const Smol::DOM::Value& dom,
        Smol::World& world
    ){
        using namespace Smol;

        auto possessor = dom.get<Str>("possessed_by");
        if(!possessor.has_value())
            return;

        auto actor = world.FindActorByName(*possessor);
        if(actor == nullptr){
            LOG_ERROR("Controller {} not found. (Potentially invalid Actor Define Order)",
                *possessor
            );
            return;
        }

        auto controller = dynamic_cast<CharacterController*>(actor);
        if(controller == nullptr){
            LOG_ERROR("Actor {} is not CharacterController",
                *possessor
            );
            return;
        }

        pawn.PossessedBy(*controller);
    }

    void createActors(
        Smol::World& world,
        const Smol::DOM::Value& dom,
        const std::filesystem::path& contentRoot
    ){
        using namespace Smol;

        dom.forEach("actors", [&](const DOM::Value& node){
            auto type = node.get<Str>("type");
            auto name = node.get<Str>("name")
                .value_or({});

            if(!type) return;

            Actor* actor = nullptr;

            if(*type == "Actor"){
                actor = createActor<Actor>(world, node, name);
            }
            else if(*type == "CharacterController"){
                actor = createActor<CharacterController>(world, node, name);
            }
            else if(*type == "Pawn"){
                actor = createActor<Pawn>(world, node, name);
            }
            else{
                // User-defined Actor
                actor = world.SpawnActor(*type, name);

                ApplyProperties(actor, node);
            }

            if(actor == nullptr){
                LOG_ERROR("Create Actor \"{}\" from World Failed.", *type);
                return;
            }

            if(actor->IsA("Pawn")){
                applyPossessedBy(*static_cast<Pawn*>(actor), node, world);
            }

            node.forEach("components", [&](const DOM::Value& n){
                attachComponent(*actor, n, contentRoot);
            });
        });
    }

    // TODO.
    struct RainCB{
        Smol::f32 elapsedTime;
        Smol::f32 aspect = Smol::detail::aspect;
        Smol::f32 intensity = 0.3f;
        Smol::f32 speed = 1.0f;
        Smol::Vec3 color{0.7f, 0.8f, 1.0f};
        Smol::f32 slant = 0.05f;
    };
}

namespace Smol
{
    struct UIContext{};

    AppMainLoop::AppMainLoop(
        const AppConfig& config,
        OS& os,
        RHIDevice& device
    )
        : resourceRegistry(
            parseTomlFile(config.resourceManifest()),
            config.project.content_root,
            std::make_unique<ImmediateResourceLoader>(device, config.project.content_root)
        )
        , inputManager(
            loadTomlFile<InputConfig>(config.defaultInputPath()),
            &os.GetInputProvider()
        )
        , spriteRenderer(device, resourceRegistry.GetSpriteManager())
        , shapeRenderer(device)
        , widgetRenderer(os.GetWindow().GetWindow(), device)
        , widget(Checkbox{
            .label = "Debug",
            .onChanged = [&world = world](UIContext&, bool v){
                LOG_WARN("Checked! {}", v);
                world.SetDebugState(v);
            },
            .v = false
        })
        , world(EngineService{
            .spriteManager = &resourceRegistry.GetSpriteManager(),
            .inputManager = &inputManager,
            .spriteRenderer = &spriteRenderer,
            .shapeRenderer = &shapeRenderer,
        })
    {
        const auto contentRoot = config.project.content_root;
        const auto dom = parseTomlFile(config.startupScenePath());

        createActors(world, dom, contentRoot);

        resourceRegistry.DrainAllCompletions();

        // TODO.
        scene = device.CreateTexture(
            RHITextureCreateDesc{
                .width = os.GetWindow().GetWidth(),
                .height = os.GetWindow().GetHeight(),
                .format = RHIPixelFormat::RGBA8_UNORM,
                .usage = combine(
                    RHITextureUsage::AllowRenderTarget,
                    RHITextureUsage::AllowShaderRead
                )
            }
        );
        linearClamp = device.CreateSampler(LINEAR_CLAMP_SAMPLER);

        pipeline = device.CreatePipelineState(RHIGraphicsPipelineStateDesc{
            .topology = RHIPrimitiveTopology::TriangleStrip,
            .vertexShaderPath = "Engine/Shader/FullscreenQuad.vert.hlsl",
            .vertexShaderEntryPoint = "vs_main",
            .rasterizer = RHIRasterizerState{
                .frontCounterClockwise = false
            },
            .fragmentShaderPath = "Engine/Shader/Rain.pixel.hlsl",
            .fragmentShaderEntryPoint = "ps_main",
            .renderTargetFormats = {
                RHIPixelFormat::RGBA8_UNORM
            },
            .renderTargetCount = 1
        });

        RainCB initState{
            .elapsedTime = 0.0f,
            .aspect = detail::aspect,
            .intensity = 0.3f,
            .speed = 1.0f,
            .color = {0.7f, 0.8f, 1.0f},
            .slant = 0.05f
        };

        rainBuf = device.CreateBuffer(RHIBufferCreateDesc{
            .size = sizeof(RainCB),
            .usage = RHIBufferUsage::ConstantBuffer,
            .access = RHIMemoryAccess::CPUWrite,
            .initialData = &initState
        }, "RainCB");
    }

    bool AppMainLoop::Initialize(){
        timer.Reset();

        return true;
    }

    bool AppMainLoop::Update(){
        timer.NewFrame();
        auto deltaTime = timer.GetDeltaTime();

        inputManager.NewFrame();
        world.Update(static_cast<f32>(deltaTime));

        RainCB rainCB{
            .elapsedTime = static_cast<f32>(timer.GetElapsedTime()),
            .aspect = detail::aspect,
            .intensity = 1.0f,
            .speed = 1.0f,
            .color = {0.7f, 0.8f, 1.0f},
            .slant = 0.15f
        };
        rainBuf->Upload(&rainCB, sizeof(RainCB));

        return true;
    }

    bool AppMainLoop::Render(CommandListPool& pool, RHISwapchain& swapchain){
        // for single thread model, use single cmdList.
        auto& cmdList = pool.Acquire();
        cmdList.Begin();

        // TODO. use integrated Renderer class
        // Begin RenderPass for backbuffer
        RHIClearColor backbufferClearColor{.v = {
            0.5f, 0.5f, 0.5f, 1.0f
        }};
        RHITexture* renderTargets[1] = {scene.get()};
        cmdList.BeginRenderPass(renderTargets,
            backbufferClearColor,
            nullptr,
            {},
            RHILoadAction::Clear
        );
        cmdList.SetViewport(RHIViewport{
            .x = 0, .y = 0,
            // fill all backbuffer
            .width = static_cast<f32>(swapchain.GetWidth()),
            .height = static_cast<f32>(swapchain.GetHeight()),
            .minDepth = 0, .maxDepth = 1
        });
        spriteRenderer.Draw(cmdList);

        shapeRenderer.Draw(swapchain);

        UIContext uiContext{};
        widgetRenderer.Draw(
            "Root Widget",
            widget,
            uiContext,
            cmdList,
            &swapchain
        );

        cmdList.EndRenderPass();
        // End RenderPass for backbuffer

        // cmdList.Copy(*scene, swapchain);

        cmdList.BeginRenderPass(swapchain,
            backbufferClearColor,
            nullptr,
            {},
            RHILoadAction::Load
        );
        cmdList.SetViewport(RHIViewport{
            .x = 0, .y = 0,
            // fill all backbuffer
            .width = static_cast<f32>(swapchain.GetWidth()),
            .height = static_cast<f32>(swapchain.GetHeight()),
            .minDepth = 0, .maxDepth = 1
        });

        cmdList.SetPipelineState(*pipeline);

        cmdList.SetConstantBuffer(
            *rainBuf,
            0,
            RHIShaderStage::FragmentShader
        );
        cmdList.SetTexture(
            *scene,
            0,
            RHIBindingAccess::ReadOnly,
            RHIShaderStage::FragmentShader
        );
        cmdList.SetSampler(*linearClamp, 0, RHIShaderStage::FragmentShader);

        cmdList.Draw(4);

        cmdList.EndRenderPass();

        cmdList.Close();
        return true;
    }

    void AppMainLoop::Finalize(){

    }
}
