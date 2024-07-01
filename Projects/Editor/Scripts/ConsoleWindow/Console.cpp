
#include <iostream>
#include <vector>
#include "ModuleStateFactory.hpp"
#include "imgui.h"
#include "imgui_stdlib.h"
#include <sstream>
#include <fstream>

struct ConsoleState : public IModuleState {


    struct Entry {
        std::string name;
        int value;
    };

    float sliderValue = 50.0f;
    float sliderValue2 = 0.5f;
    std::string name;

    float time = 0;
    bool showInfoWindow = false;

    int numberOfFrames = 0;

    int currentFps = 0;

    float timer = 0;

    std::vector<Entry> entries;

    std::string code;

    void Initialize(EngineContext& context) override {

    }

    void OnGui() override {

        ImGui::Begin("ConsoleWindow");

        ImGui::SliderFloat("Value", &sliderValue, 0, 100);
        ImGui::SliderFloat("Percentage", &sliderValue2, 0, 1);

        ImGui::InputText("Name", &name);

        if (!showInfoWindow && ImGui::Button("Show info window")) {
            showInfoWindow = true;
        } else if (showInfoWindow && ImGui::Button("Hide info window")) {
            showInfoWindow = false;
        }

        ImGui::End();


        if (showInfoWindow) {
            ImGui::Begin("Info Window");

            ImGui::Text("This is the info window!");


            std::stringstream s;
            s << currentFps;

            std::string t = "Fps: " + s.str();
            auto* ttt = t.c_str();

            ImGui::Text("%s", t.c_str());


            ImGui::End();


            ImGui::Begin("Extra Window");

            if (ImGui::Button("Add entry")) {
                entries.push_back({});
            }

            if (ImGui::Button("Delete All")) {
                ImGui::OpenPopup("Delete All?");
            }

            static bool dont_ask_me_next_time = false;

            if (!dont_ask_me_next_time && ImGui::BeginPopupModal("Delete All?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!");
                ImGui::Separator();

                //static int unused_i = 0;
                //ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

                //ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
                //ImGui::PopStyleVar();

                if (ImGui::Button("OK", ImVec2(120, 0))) {
                    entries.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }


            for (int i = 0; i < entries.size(); ++i) {
                std::stringstream s;
                s << "Entry #" << (i + 1);

                ImGui::InputText(s.str().c_str(), &entries[i].name);
                ImGui::SameLine();
                ImGui::PushID(&entries[i]);
                if (ImGui::Button("-")) {
                    entries.erase(entries.begin() + i);
                    i--;
                }
                ImGui::PopID();

                /* code */
            }



            ImGui::End();

            DrawCodeEditor();

        }


    }

    void Update(float dt) override {

        timer += dt;

        if (timer > 1.0f) {
            currentFps = numberOfFrames;
            numberOfFrames = 0;
            timer = 0.0f;
        }
        numberOfFrames++;
    }

    void Render() override {

    }

    void DrawCodeEditor() {
        ImGui::Begin("Code editor");

        auto oldCode = code;
        ImGui::InputTextMultiline("Code", &code);
        if (code!=oldCode){
            SaveToTextFile();
        }

        if (ImGui::Button("Save")) {
            SaveToTextFile();
        }

        ImGui::End();
    }

    void SaveToTextFile() {
        std::ofstream file;
        file.open("test.txt");
        file << code;
    }


};

extern "C" {
    IModuleState* CreateModuleState(ModuleStateFactory* factory) {
        return factory->CreateState<ConsoleState>();
    }

    void DeleteModuleState(IModuleState* state) {
        delete state;
    }
}