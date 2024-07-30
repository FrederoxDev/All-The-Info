#pragma once
#include <minecraft/src/common/world/phys/Vec3.hpp>
#include <imgui.h>

void RenderVec3(const char* label, const Vec3& vec) {
    float values[3] = { vec.x, vec.y, vec.z };

    ImGui::Text("%s", label);

    float inputFieldWidth = 70.0f;

    ImGui::SameLine();
    ImGui::PushItemWidth(inputFieldWidth);
    ImGui::InputFloat("##x", &values[0], 0.0f, 0.0f, "%.2f", ImGuiInputTextFlags_ReadOnly);
    ImGui::SameLine();
    ImGui::InputFloat("##y", &values[1], 0.0f, 0.0f, "%.2f", ImGuiInputTextFlags_ReadOnly);
    ImGui::SameLine();
    ImGui::InputFloat("##z", &values[2], 0.0f, 0.0f, "%.2f", ImGuiInputTextFlags_ReadOnly);
    ImGui::PopItemWidth();
}

void RenderVec2(const char* label, const Vec2& vec) {
    float values[2] = { vec.x, vec.y };

    ImGui::Text("%s", label);

    float inputFieldWidth = 70.0f;

    ImGui::SameLine();
    ImGui::PushItemWidth(inputFieldWidth);
    ImGui::InputFloat("##x", &values[0], 0.0f, 0.0f, "%.2f", ImGuiInputTextFlags_ReadOnly);
    ImGui::SameLine();
    ImGui::InputFloat("##y", &values[1], 0.0f, 0.0f, "%.2f", ImGuiInputTextFlags_ReadOnly);
    ImGui::PopItemWidth();
}