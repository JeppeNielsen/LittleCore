//
// Created by Jeppe Nielsen on 28/12/2025.
//


#include "GuiResourceDrawers.hpp"
#include "TupleHelper.hpp"
#include "PrefabSystem.hpp"
#include "MainState.hpp"
#include <SDL3/SDL.h>
#include "PrefabDrawer.hpp"
#include "PrefabExposedComponentsDrawer.hpp"
#include "DefaultSimulation.hpp"
#include "SDLInputHandler.hpp"
#include "BgfxRenderer.hpp"
#include "DefaultResourceManager.hpp"
#include <glm/glm.hpp>
#include <thread>
#include "EntityGuiDrawer.hpp"
#include "SystemBase.hpp"
#include "InputRotationSystem.hpp"
#include "MovableSystem.hpp"
#include "PickingSystem.hpp"
#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "MeshLoader.hpp"
#include "RegistrySerializer.hpp"
#include "MetaHelper.hpp"
#include "FileHelper.hpp"
#include "PrefabExposedComponents.hpp"
#include "EditorSimulationRegistry.hpp"
#include "ImGuiController.hpp"

#include <thread>

using namespace LittleCore;

struct MainState::Parameters {

    BGFXRenderer renderer;
    EditorSimulationContext editorSimulationContext;
    EditorSimulationRegistry editorSimulationRegistry;
    NetimguiClientController netimguiClientController;
    ImGuiController gui;
    Project project;
    ProjectWindow projectWindow;
    DefaultResourceManager resourceManager;
    EntityGuiDrawerContext drawerContext;
    EntityGuiDrawerBase* entityGuiDrawer;
    RegistrySerializerBase* registrySerializer;

    ~Parameters() {
        delete entityGuiDrawer;
        delete registrySerializer;
    }

    Parameters() :
            drawerContext(resourceManager),
            editorSimulationContext(renderer, netimguiClientController),
            editorSimulationRegistry(editorSimulationContext),
            resourceManager(project.resourcePathMapper) {}


    void Initialize(void* mainWindow, const ImGuiController::RenderFunction& onGui) {

        gui.Initialize(mainWindow, onGui);

        gui.LoadFont("/Users/jeppe/Jeppes/LittleCore/Projects/TestImGui/Source/Fonts/LucidaG.ttf", 12);

        netimguiClientController.Start();
        netimguiClientController.Connect("Test client", "localhost");

        while (netimguiClientController.IsConnectionPending()) {
            std::this_thread::sleep_for(std::chrono::milliseconds (16));
        }
        if (!netimguiClientController.IsConnected()) {
            std::cout << "couldn't connect\n";
        }

        project.rootPath = "/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimguiClient/Source/Assets/";
        project.resourcePathMapper.RefreshFromRootPath(project.rootPath);

        resourceManager.CreateLoaderFactory<ShaderResourceLoaderFactory>();
        resourceManager.CreateLoaderFactory<TextureResourceLoaderFactory>();
        resourceManager.CreateLoaderFactory<MeshResourceLoaderFactory>();
        resourceManager.CreateLoaderFactory<FontResourceLoaderFactory>();


    }

    void SetGuiDrawer(EntityGuiDrawerBase* entityGuiDrawer) {
        this->entityGuiDrawer = entityGuiDrawer;
        editorSimulationContext.guiDrawer = entityGuiDrawer;
        entityGuiDrawer->Initialize(&drawerContext);
    }

    void SetRegistrySerializer(RegistrySerializerBase* registrySerializer) {
        this->registrySerializer = registrySerializer;
        drawerContext.registrySerializer = registrySerializer;
        resourceManager.CreateLoaderFactory<PrefabResourceLoaderFactory>(*registrySerializer, &resourceManager);
    }

    void UpdateEditorSimulation(float dt) {
        EditorSimulation* currentSimulation;
        if (editorSimulationRegistry.TryGetFirst(&currentSimulation)) {
            currentSimulation->Update(dt);
        }
    }

    void DrawUI() {

        projectWindow.Draw(project, resourceManager, editorSimulationRegistry);

        EditorSimulation* currentSimulation;
        if (editorSimulationRegistry.TryGetFirst(&currentSimulation)) {
            currentSimulation->DrawGUI();
        }
    }

    void AddSimulation(SimulationBase& simulation) {
        editorSimulationRegistry.AddSimulation(simulation);
        simulation.SetResources(*registrySerializer, resourceManager);
    }

};


MainState::MainState() {
    parameters = new Parameters();
}

MainState::~MainState() {
    delete parameters;
}

void MainState::Initialize() {
    parameters->Initialize(mainWindow, [this](){
        ImGui::DockSpaceOverViewport();
        parameters->DrawUI();
        OnGui();
    });
    OnInitialize();
}

void MainState::Update(float dt) {
    parameters->UpdateEditorSimulation(dt);
    OnUpdate(dt);
}

void MainState::Render() {
    parameters->gui.Render();
    OnRender();
}

void MainState::HandleEvent(void* event) {
    parameters->gui.HandleEvent(event);
}

void MainState::OnGui() {
    ImGui::Begin("Window");
    ImGui::End();
}

void MainState::AddSimulation(SimulationBase& simulation) {
    parameters->AddSimulation(simulation);
}

void MainState::AddEntityGuiDrawer(EntityGuiDrawerBase* entityGuiDrawerBase) {
    parameters->SetGuiDrawer(entityGuiDrawerBase);
}

void MainState::AddRegistrySerializer(RegistrySerializerBase* registrySerializerBase) {
    parameters->SetRegistrySerializer(registrySerializerBase);
}

std::string MainState::Save(const entt::registry& registry) const {
    SerializationContext context {
        .resourceManager = &parameters->resourceManager
    };
    return parameters->registrySerializer->Serialize(registry, context);
}

std::string MainState::Load(entt::registry& registry, const std::string& data) const {
    SerializationContext context {
        .resourceManager = &parameters->resourceManager
    };
    return parameters->registrySerializer->Deserialize(registry, data, context);
}
