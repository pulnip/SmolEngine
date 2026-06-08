#include "AppConfig.hpp"
#include "AppMainLoop.hpp"
#include "CharacterController.hpp"
#include "ClassRegistry.hpp"
#include "CommandListPool.hpp"
#include "InputComponent.hpp"
#include "LogLocal.hpp"
#include "MoveComponent.hpp"
#include "OS.hpp"
#include "Pawn.hpp"
#include "RHICommandList.hpp"
#include "RHIDevice.hpp"
#include "RHITexture.hpp"
#include "RHISwapchain.hpp"
#include "ResourceManager.hpp"
#include "SpriteComponent.hpp"
#include "SpriteAnimComponent.hpp"
#include "TomlLoader.hpp"

namespace{
    template<std::derived_from<Smol::Component> T>
    T* createComponent(
        Smol::Actor& actor,
        const Smol::DOM::Value& dom
    ){
        auto component = actor.AddComponent<T>();
        auto& desc = Smol::ClassRegistry::Get().DescFor<T>();

        ApplyProperties(desc, &actor, dom);
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
        else if(*type == "MoveComponent"){
            createComponent<MoveComponent>(actor, dom);
        }
        else if(*type == "SpriteAnimComponent"){
            auto component = createComponent<SpriteAnimComponent>(actor, dom);
            component->OnAttach(dom, contentRoot);
        }
        else if(*type == "SpriteComponent"){
            auto component = createComponent<SpriteComponent>(actor, dom);
            component->OnAttach(dom, contentRoot);
        }
        else{
            // user-defined Component
            actor.AddComponent(*type);

            // TODO. apply property to user-defined component
        }
    }

    template<std::derived_from<Smol::Actor> T>
    T* createActor(
        Smol::World& world,
        const Smol::DOM::Value& dom,
        const Smol::Str& name
    ){
        auto actor = world.SpawnActor<T>(name);
        auto& desc = Smol::ClassRegistry::Get().DescFor<T>();

        ApplyProperties(desc, actor, dom);
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

                // TODO. apply property to user-defined actor
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

}

namespace Smol
{
    AppMainLoop::AppMainLoop(
        const AppConfig& config,
        OS& os,
        RHIDevice& device
    )
        : resourceLoader(device, config.project.content_root)
        , spriteManager(resourceLoader)
        // TODO. use toml later
        , inputManager(
            loadTomlFile<InputConfig>(config.defaultInputPath()),
            &os.GetInputProvider()
        )
        , spriteRenderer(device, spriteManager)
        , world(EngineService{
            .spriteManager = &spriteManager,
            .inputManager = &inputManager,
            .spriteRenderer = &spriteRenderer,
        })
    {
        const auto contentRoot = config.project.content_root;
        const auto dom = parseTomlFile(config.startupScenePath());

        createActors(world, dom, contentRoot);

        spriteManager.DrainCompletions();
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
        cmdList.BeginRenderPass(swapchain,
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

        cmdList.EndRenderPass();
        // End RenderPass for backbuffer

        cmdList.Close();
        return true;
    }

    void AppMainLoop::Finalize(){

    }
}
