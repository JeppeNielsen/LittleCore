#include "SizableSystem.hpp"
#include "Mesh.hpp"
#include "Sizable.hpp"
#include <iostream>


using namespace LittleCore;

void SizableSystem::Update(float dt) {

    for(auto [entity, mesh, sizeable] : registry.view<Mesh, Sizable>().each()) {

        if (mesh.handle) {
            mesh.handle.Clear();
        }

        
        if (mesh.triangles.size() != 6) {
            mesh.triangles = {0,1,2,0,2,3};
        }
        
        mesh.vertices = {
                { {0,0,0}, 0xFFFFFFFF, {0,0} },
                { {sizeable.size.x,0,0}, 0xFFFFFFFF, {1,0} },
                { {sizeable.size.x,sizeable.size.y,0}, 0xFFFFFFFF, {1,1} },
                { {0,sizeable.size.y,0}, 0xFFFFFFFF, {0,1} }
            };
            
        registry.patch<Mesh>(entity);
        
    }
    
}








