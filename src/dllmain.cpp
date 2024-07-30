#include "dllmain.h"
#include <cmath>
#include <chrono>
#include <imgui.h>
#include <imgui_mc/imgui_impl_mc.hpp>
#include <implot.h>
#include <minecraft/src-client/common/client/renderer/BaseActorRenderContext.hpp>
#include <minecraft/src-client/common/client/renderer/TextureGroup.hpp>
#include <minecraft/src-client/common/client/game/MinecraftGame.hpp>
#include <minecraft/src-client/common/client/gui/ScreenView.hpp>
#include <minecraft/src-deps/minecraftrenderer/renderer/TextureContainer.hpp>
#include <minecraft/src-deps/input/MouseDevice.hpp>
#include <amethyst/runtime/events/RenderingEvents.hpp>
#include <amethyst/runtime/ModContext.hpp>
#include <minecraft/src-client/common/client/gui/gui/VisualTree.hpp>
#include <minecraft/src-client/common/client/gui/gui/UIControl.hpp>
#include <modules/Profiler.hpp>
#include <modules/EntityInfo.hpp>
#include <minecraft/src/common/world/level/BlockSource.hpp>

extern mce::TextureGroup* textureGroup;

void InitImGui(MinecraftUIRenderContext& ctx) {
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

	ImGui_ImplMc_Init(ctx);
}

bool showFPS = false;
bool showEntityInfo = true;

void RenderImGui(ScreenView* screen, MinecraftUIRenderContext* ctx) {
	ImGui_ImplMc_NewFrame(*ctx);
	ImGui::NewFrame();

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Options")) {
			ImGui::MenuItem("Show FPS", NULL, &showFPS);
			ImGui::MenuItem("Entity Info", NULL, &showEntityInfo);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (showFPS) RenderProfiler(*screen);
	if (showEntityInfo) RenderEntityList(*ctx->mClient);
	

	ImGui::EndFrame();
	ImGui::Render();

	ImGui_ImplMc_RenderDrawData(ImGui::GetDrawData(), *ctx);
}

void AfterRenderUI(AfterRenderUIEvent& event) {
	static bool once = false;
	if (!once) {
		InitImGui(event.ctx);
		once = true;
	}

	if (event.screen.visualTree->mRootControlName->mName == "hud_screen" || event.screen.visualTree->mRootControlName->mName == "pause_screen") {
		RenderImGui(&event.screen, &event.ctx);
	}
}

SafetyHookInline _uploadTexture;

BedrockTexture* uploadTexture(mce::TextureGroup* group, ResourceLocation* resource, mce::TextureContainer* texture, std::optional<std::string_view> debugName) {
	static bool once = false;

	if (!once) {
		textureGroup = group;
		once = true;
	}

	return _uploadTexture.call<BedrockTexture*>(group, resource, texture, debugName);
}

SafetyHookInline _feed;

void feed(MouseDevice* mouse, char actionButtonId, char buttonData, short x, short y, short dx, short dy, bool forceMotionlessPointer) {
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent(x, y);

	if (actionButtonId == 4) {
		float normalizedScroll = buttonData / 120.0f;
		//io.AddMouseWheelEvent(0.0f, normalizedScroll);
		io.MouseWheel = normalizedScroll;
	}
	else if (actionButtonId != 0) {
		io.AddMouseButtonEvent(actionButtonId - 1, buttonData == 1);
	}

	// ImGui wants the mouse...
	if (io.WantCaptureMouse) return;

	// Pass mouse input back into the underlying game.
	_feed.thiscall(mouse, actionButtonId, buttonData, x, y, dx, dy, forceMotionlessPointer);
}

SafetyHookInline _unloadAllTextures;

void unloadAllTextures(mce::TextureGroup* self) {
	ImGui_ImplMc_OnUnloadAllTextures();
	_unloadAllTextures.thiscall(self);
}

ModFunction void Initialize(AmethystContext& ctx)
{
	Amethyst::InitializeAmethystMod(ctx);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();

	auto& events = Amethyst::GetEventBus();
	events.AddListener<AfterRenderUIEvent>(&AfterRenderUI);

	auto& hooks = Amethyst::GetHookManager();

	hooks.RegisterFunction<&uploadTexture>(SigScan("40 55 53 56 57 41 54 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 49 8B F9 4D 8B F8 4C 8B F2 48 8B F1"));
	hooks.CreateHook<&uploadTexture>(_uploadTexture, &uploadTexture);

	hooks.RegisterFunction<&MouseDevice::feed>("48 8B C4 48 89 58 ? 48 89 68 ? 48 89 70 ? 57 41 54 41 55 41 56 41 57 48 83 EC ? 44 0F B7 BC 24");
	hooks.CreateHook<&MouseDevice::feed>(_feed, &feed);

	hooks.RegisterFunction<&mce::TextureGroup::unloadAllTextures>("48 89 5C 24 ? 57 48 83 EC ? 48 8B 99 ? ? ? ? 48 8B F9 48 8B 1B 80 7B ? ? 75 ? 0F 1F 00 48 8D 53");
	hooks.CreateHook<&mce::TextureGroup::unloadAllTextures>(_unloadAllTextures, &unloadAllTextures);
}