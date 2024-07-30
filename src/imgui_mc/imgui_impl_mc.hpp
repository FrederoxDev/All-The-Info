#pragma once
#include <imgui.h>

class MinecraftUIRenderContext;

#ifndef IMGUI_DISABLE 

IMGUI_IMPL_API bool ImGui_ImplMc_Init(MinecraftUIRenderContext& tess);
IMGUI_IMPL_API void ImGui_ImplMc_Shutdown();
IMGUI_IMPL_API void ImGui_ImplMc_NewFrame(MinecraftUIRenderContext& context);
IMGUI_IMPL_API void ImGui_ImplMc_RenderDrawData(ImDrawData* drawData, MinecraftUIRenderContext& context);
void ImGui_ImplMc_OnUnloadAllTextures();

#endif // #ifndef IMGUI_DISABLE