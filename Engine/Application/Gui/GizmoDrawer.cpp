//
// Created by Jeppe Nielsen on 29/09/2025.
//

#include "GizmoDrawer.hpp"
#include "Camera.hpp"
#include "WorldTransform.hpp"
#include "LocalTransform.hpp"
#include <algorithm>
#include <cmath>
#include <glm/gtx/matrix_decompose.hpp>
#include "Hierarchy.hpp"
#include "Sizable.hpp"

using namespace LittleCore;

void GizmoDrawer::Begin() {
    min = ImGui::GetItemRectMin();
    max = ImGui::GetItemRectMax();
    ImGuizmo::BeginFrame();
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(min.x, min.y, max.x - min.x, max.y - min.y);
}

void GizmoDrawer::DrawGizmo(GizmoDrawerContext& context,
                            entt::registry& cameraRegistry, entt::entity cameraEntity,
                            entt::registry& objectRegistry, entt::entity objectEntity,
                            ImGuizmo::OPERATION operation) {
    if (!objectRegistry.all_of<WorldTransform>(objectEntity)) {
        return;
    }

    ImVec2 size = {max.x - min.x, max.y - min.y};

    ImGuizmo::PushID((int)objectEntity);

    auto& camera = cameraRegistry.get<Camera>(cameraEntity);
    auto& cameraWorldTransform = cameraRegistry.get<WorldTransform>(cameraEntity);



    auto& quadWorldTransform = objectRegistry.get<WorldTransform>(objectEntity);

    auto viewPtr = glm::value_ptr(cameraWorldTransform.worldInverse);
    auto projectionPtr = glm::value_ptr(camera.GetProjection(size.x/size.y));
    auto model = glm::value_ptr(quadWorldTransform.world);
    ImGuizmo::Manipulate(viewPtr, projectionPtr, operation, ImGuizmo::MODE::LOCAL, model);

    bool isUsing = ImGuizmo::IsUsing();

    context.wasActive |= isUsing;
    context.wasHovered |= ImGuizmo::IsOver();

    if (isUsing) {
        auto& quadLocalTransform = objectRegistry.get<LocalTransform>(objectEntity);
        mat4x4 worldToDecompose;
        Hierarchy* hierarchy = objectRegistry.try_get<Hierarchy>(objectEntity);
        if (!hierarchy || !objectRegistry.valid(hierarchy->parent)) {
            worldToDecompose = quadWorldTransform.world;
        } else {
            WorldTransform& parentWorld = objectRegistry.get<WorldTransform>(hierarchy->parent);
            worldToDecompose = parentWorld.worldInverse * quadWorldTransform.world;
        }

        glm::vec3 skew;
        glm::vec4 perspective;
        if (glm::decompose(worldToDecompose, quadLocalTransform.scale, quadLocalTransform.rotation,
                            quadLocalTransform.position, skew, perspective)) {
            objectRegistry.patch<LocalTransform>(objectEntity);
        }
    }

    ImGuizmo::PopID();
}

void GizmoDrawer::DrawSizableBounds(GizmoDrawerContext& context,
                                    entt::registry& cameraRegistry, entt::entity cameraEntity,
                                    entt::registry& objectRegistry, entt::entity objectEntity,
                                    Sizable& sizable) {
    if (!objectRegistry.all_of<WorldTransform, LocalTransform>(objectEntity)) {
        return;
    }

    ImVec2 size = {max.x - min.x, max.y - min.y};

    ImGuizmo::PushID((int)objectEntity);

    auto& camera = cameraRegistry.get<Camera>(cameraEntity);
    auto& cameraWorldTransform = cameraRegistry.get<WorldTransform>(cameraEntity);
    auto& objectWorldTransform = objectRegistry.get<WorldTransform>(objectEntity);
    auto& objectLocalTransform = objectRegistry.get<LocalTransform>(objectEntity);
    const bool isActiveEntity = activeSizableEntity == objectEntity;
    const glm::vec2 dragStartSize = isActiveEntity ? activeSizableSize : sizable.size;
    glm::mat4 manipulatedWorld = isActiveEntity ? activeSizableWorld : objectWorldTransform.world;

    const float localBounds[6] = {0.0f, 0.0f, 0.0f, dragStartSize.x, dragStartSize.y, 0.0f};

    ImGuizmo::Manipulate(glm::value_ptr(cameraWorldTransform.worldInverse),
                         glm::value_ptr(camera.GetProjection(size.x / size.y)),
                         ImGuizmo::BOUNDS,
                         ImGuizmo::MODE::LOCAL,
                         glm::value_ptr(manipulatedWorld),
                         nullptr,
                         nullptr,
                         localBounds,
                         nullptr);

    const bool isUsing = ImGuizmo::IsUsing();
    context.wasActive |= isUsing;
    context.wasHovered |= ImGuizmo::IsOver();

    if (isUsing) {
        if (activeSizableEntity != objectEntity) {
            activeSizableEntity = objectEntity;
            activeSizableSize = sizable.size;
            activeSizableScale = objectLocalTransform.scale;
            activeSizableRotation = objectLocalTransform.rotation;
            activeSizableWorld = objectWorldTransform.world;
        }

        glm::mat4 localMatrix = manipulatedWorld;

        if (Hierarchy* hierarchy = objectRegistry.try_get<Hierarchy>(objectEntity); hierarchy && objectRegistry.valid(hierarchy->parent)) {
            const WorldTransform& parentWorld = objectRegistry.get<WorldTransform>(hierarchy->parent);
            localMatrix = parentWorld.worldInverse * manipulatedWorld;
        }

        const glm::vec3 localOrigin = glm::vec3(localMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        const glm::vec3 localX = glm::vec3(localMatrix * glm::vec4(dragStartSize.x, 0.0f, 0.0f, 1.0f));
        const glm::vec3 localY = glm::vec3(localMatrix * glm::vec4(0.0f, dragStartSize.y, 0.0f, 1.0f));

        const glm::mat3 rotationMatrix = glm::mat3_cast(activeSizableRotation);
        const glm::vec3 axisX = glm::normalize(glm::vec3(rotationMatrix[0]));
        const glm::vec3 axisY = glm::normalize(glm::vec3(rotationMatrix[1]));

        const float safeScaleX = std::abs(activeSizableScale.x) > 0.0001f ? std::abs(activeSizableScale.x) : 1.0f;
        const float safeScaleY = std::abs(activeSizableScale.y) > 0.0001f ? std::abs(activeSizableScale.y) : 1.0f;

        const float newWidth = std::max(0.001f, std::abs(glm::dot(localX - localOrigin, axisX)) / safeScaleX);
        const float newHeight = std::max(0.001f, std::abs(glm::dot(localY - localOrigin, axisY)) / safeScaleY);

        objectLocalTransform.position = localOrigin;
        objectRegistry.patch<LocalTransform>(objectEntity);
        if (sizable.size.x != newWidth || sizable.size.y != newHeight) {
            sizable.size = {newWidth, newHeight};
            objectRegistry.patch<Sizable>(objectEntity);
        }
        activeSizableWorld = manipulatedWorld;
    } else if (activeSizableEntity == objectEntity) {

        activeSizableEntity = entt::null;
        activeSizableSize = {0.0f, 0.0f};
        activeSizableScale = {1.0f, 1.0f, 1.0f};
        activeSizableRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        activeSizableWorld = glm::mat4(1.0f);
    }

    ImGuizmo::PopID();
}

bool GizmoDrawer::IsSizableBoundsActive(entt::entity entity) const {
    return activeSizableEntity == entity;
}

ImGuizmo::OPERATION GizmoDrawer::DrawOperationUI(ImGuizmo::OPERATION currentOperation) {
    return ImGuizmo::TRANSLATE;
}

static bool ClipSegmentHomogeneous(glm::vec4& a, glm::vec4& b, bool depthZeroToOne)
{
    auto clipPlane = [&](float fa, float fb, glm::vec4& pa, glm::vec4& pb) -> bool {
        // If both outside on same side → reject
        if (fa > 0.0f && fb > 0.0f) return false;
        // If both inside → keep
        if (fa <= 0.0f && fb <= 0.0f) return true;
        // Intersect: move the outside endpoint onto the plane
        float t = fa / (fa - fb);
        glm::vec4 p = pa + t * (pb - pa);
        if (fa >= 0.0f) pa = p; else pb = p;
        return true;
    };

    // Helper: evaluate plane inequalities of form (±x - w) ≤ 0, (±y - w) ≤ 0, and z-range.
    auto clipAxis = [&](glm::vec4& a, glm::vec4& b, int axis) -> bool {
        float ax = (&a.x)[axis], bx = (&b.x)[axis];
        // +axis:  x - w <= 0
        if (!clipPlane(ax - a.w, bx - b.w, a, b)) return false;
        // -axis: -x - w <= 0  (equiv. -(x + w) <= 0 ⇒ x >= -w)
        if (!clipPlane(-(ax + a.w), -(bx + b.w), a, b)) return false;
        return true;
    };

    // x and y
    if (!clipAxis(a, b, 0)) return false; // x
    if (!clipAxis(a, b, 1)) return false; // y

    // z
    if (depthZeroToOne) {
        // 0 <= z <= w  → planes: -z <= 0  and  z - w <= 0
        if (!clipPlane(-a.z, -b.z, a, b)) return false;         // z >= 0
        if (!clipPlane(a.z - a.w, b.z - b.w, a, b)) return false; // z <= w
    } else {
        // -w <= z <= w → planes: -(z + w) <= 0  and  z - w <= 0
        if (!clipPlane(-(a.z + a.w), -(b.z + b.w), a, b)) return false; // z >= -w
        if (!clipPlane(a.z - a.w, b.z - b.w, a, b)) return false;       // z <= w
    }

    return true;
}


void GizmoDrawer::DrawCameraFrustum(entt::registry& cameraRegistry, entt::entity cameraEntity,
                                    entt::registry& objectRegistry, entt::entity objectEntity,
                                    float aspect,
                                    ImU32 color,
                                    float thickness,
                                    bool depthZeroToOne)

{
    const float zn = depthZeroToOne ? 0.0f : -1.0f;  // near  z in NDC
    const float zf = 1.0f;                           // far   z in NDC

    std::array<glm::vec4, 8> ndc = {
            glm::vec4(-1.f, -1.f, zn, 1.f), glm::vec4(+1.f, -1.f, zn, 1.f),
            glm::vec4(+1.f, +1.f, zn, 1.f), glm::vec4(-1.f, +1.f, zn, 1.f),
            glm::vec4(-1.f, -1.f, zf, 1.f), glm::vec4(+1.f, -1.f, zf, 1.f),
            glm::vec4(+1.f, +1.f, zf, 1.f), glm::vec4(-1.f, +1.f, zf, 1.f)
    };

    ImVec2 size = {max.x - min.x, max.y - min.y};

    auto& camera = cameraRegistry.get<Camera>(cameraEntity);
    auto& cameraWorldTransform = cameraRegistry.get<WorldTransform>(cameraEntity);

    auto& objectCamera = objectRegistry.get<Camera>(objectEntity);
    auto& objectCameraWorldTransform = objectRegistry.get<WorldTransform>(objectEntity);

    const glm::mat4& cameraView      = cameraWorldTransform.worldInverse;
    const glm::mat4& cameraProj      = camera.GetProjection(size.x / size.y);
    const glm::mat4& cameraVP        = cameraProj * cameraView;

    const glm::mat4& objectView      = objectCameraWorldTransform.worldInverse;
    const glm::mat4& objectProj      = objectCamera.GetProjection(aspect);
    const glm::mat4& objectVP        = objectProj * objectView;
    const glm::mat4& invObjectVP     = glm::inverse(objectVP);

    std::array<glm::vec4,8> world;
    for (int i = 0; i < 8; ++i) {
        glm::vec4 w = invObjectVP * ndc[i];
        world[i] = w / w.w; // guard w.w!=0 below if needed
    }

    auto worldToNDC = [&](const glm::vec4& w) -> glm::vec3 {
        glm::vec4 c = cameraVP * w;
        if (c.w == 0.0f) return glm::vec3(NAN); // avoid INF
        c /= c.w;
        return glm::vec3(c);
    };

    auto ndcToScreen = [&](const glm::vec3& p) -> ImVec2 {
        float sx = min.x + (p.x * 0.5f + 0.5f) * size.x;
        float sy = min.y + (1.0f - (p.y * 0.5f + 0.5f)) * size.y; // flip Y
        return ImVec2(sx, sy);
    };

    auto clipToScreen = [&](const glm::vec4& c, const ImVec2& min, const ImVec2& max) -> ImVec2 {
        glm::vec3 ndc = glm::vec3(c) / c.w;     // c.w is safe now
        float sx = min.x + (ndc.x * 0.5f + 0.5f) * (max.x - min.x);
        float sy = min.y + (1.0f - (ndc.y * 0.5f + 0.5f)) * (max.y - min.y);
        return ImVec2(sx, sy);
    };

    std::array<ImVec2, 8> screen;
    for (int i = 0; i < 8; ++i) {
        screen[i] = ndcToScreen(worldToNDC(world[i]));
    }

    auto* dl = ImGui::GetForegroundDrawList();

    auto drawClippedEdge = [&](const glm::vec4& w0, const glm::vec4& w1) {
        glm::vec4 a = cameraVP * w0;   // clip-space (pre-divide)
        glm::vec4 b = cameraVP * w1;

        // Reject if both have non-finite components (rare but defensive)
        auto finite = [](const glm::vec4& v){
            return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z) && std::isfinite(v.w);
        };
        if (!finite(a) || !finite(b)) return;

        if (!ClipSegmentHomogeneous(a, b, depthZeroToOne)) return; // fully outside

        // Perspective divide *after* clipping
        if (a.w == 0.0f || b.w == 0.0f) return; // extra guard

        ImVec2 p0 = clipToScreen(a, min, max);
        ImVec2 p1 = clipToScreen(b, min, max);

        dl->AddLine(p0, p1, color, thickness);
    };

    dl->PushClipRect(min, max, true);
    auto line = [&](int a, int b) {
        //dl->AddLine(screen[a], screen[b], color, thickness);
        drawClippedEdge(world[a], world[b]);
    };

    // Near rectangle
    line(0,1); line(1,2); line(2,3); line(3,0);
    // Far rectangle
    line(4,5); line(5,6); line(6,7); line(7,4);
    // Connect near/far
    line(0,4); line(1,5); line(2,6); line(3,7);

    dl->PopClipRect();
}
