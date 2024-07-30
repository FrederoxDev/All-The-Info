#pragma once
#include <minecraft/src-client/common/client/player/LocalPlayer.hpp>
#include <imgui_mc/imgui_helpers.hpp>
#include <minecraft/src/common/world/level/BlockSource.hpp>
#include <minecraft/src/common/world/entity/components/ActorDefinitionIdentifierComponent.hpp>

Actor* selectedEntity = nullptr;

void RenderEntityInfo(Actor& actor) {
    ImGui::Begin("Entity Info");

    RenderVec3("Position", *actor.getPosition());
    RenderVec2("Head Rotation", *actor.getHeadRot());

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Components:")) {
        for (auto&& curr : actor.mEntityContext.mEnttRegistry.storage()) {
            entt::id_type id = curr.first;
            auto& storage = curr.second;
            entt::type_info ctype = storage.type();

            if (storage.contains(actor.mEntityContext.mEntity)) {
                ImGui::Text(ctype.name().data());
            }
        }
    }

    ImGui::End();
}

void RenderEntityList(ClientInstance& client) {
    ImGui::Begin("Entities");

    Vec3 pos = *client.getLocalPlayer()->getPosition();
    AABB aabb(pos.x - 10.0f, pos.y - 10.0f, pos.z - 10.0f, pos.x + 10.0f, pos.y + 10.0f, pos.z + 10.0f);
    auto span = client.getRegion()->fetchEntities(nullptr, aabb, true, true);

    for (auto& actor : span) {
        auto component = actor->tryGetComponent<ActorDefinitionIdentifierComponent>();
        if (component == nullptr) {
            Log::Info("null component");
            return;
        }

        if (ImGui::Selectable(component->mIdentifier.mFullName.c_str())) {
            selectedEntity = actor.get();
        }
    }

    ImGui::End();

    if (selectedEntity != nullptr) {
        RenderEntityInfo(*selectedEntity);
    }
}