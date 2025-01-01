//
// Created by Jeppe Nielsen on 30/11/2024.
//

#include "ProjectCompiler.hpp"

ProjectCompiler::ProjectCompiler(Project &project, IProjectCompilerHandler &handler) :
        project(project),
        handler(handler)
{

}


void ProjectCompiler::Compile() {
    compilationTimer.Start();
    isCompiling = true;

    taskRunner.runAsyncTask([&, this]{
        for(auto& definition : project.moduleDefinitionsManager.Definitions()) {
            LittleCore::Timer timer;
            timer.Start();
            auto result = definition.second->Build();
            auto time = timer.Stop();

            errorsFromCompilation.push_back(definition.first + " took: " + std::to_string(time) +  " seconds");

            for(auto error : result.errors) {
                errorsFromCompilation.push_back(error);
            }
        }
    }, [&, this] {

        float duration = compilationTimer.Stop();
        std::cout << "Compilation took = " << std::to_string(duration) << "\n";
        isCompiling = false;



        errorsFromCompilation.push_back("Compilation took = " + std::to_string(duration) + "\n");

        result = {};
        result.errors = errorsFromCompilation;
        errorsFromCompilation.clear();

        handler.CompilationFinished(result);

        for(auto& module : project.moduleManager.GetModules()) {
            module.second->Unload();
        }

        for(auto& module : project.moduleManager.GetModules()) {
            module.second->Load();
        }
    });
}

bool ProjectCompiler::IsCompiling() const {
    return isCompiling;
}

void ProjectCompiler::Update() {
    taskRunner.processTasks();
}

const CompilationResult &ProjectCompiler::Result() const {
    return result;
}
