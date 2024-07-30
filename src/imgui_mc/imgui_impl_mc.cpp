#include "imgui_impl_mc.hpp"
#include <minecraft/src-deps/minecraftrenderer/renderer/Mesh.hpp>
#include <minecraft/src-client/common/client/renderer/RenderMaterialGroup.hpp>
#include <minecraft/src-deps/minecraftrenderer/renderer/MaterialPtr.hpp>
#include <minecraft/src-deps/core/container/Blob.hpp>
#include <minecraft/src-deps/coregraphics/ImageDescription.hpp>
#include <minecraft/src-deps/coregraphics/ImageBuffer.hpp>
#include <minecraft/src-deps/minecraftrenderer/renderer/TextureContainer.hpp>
#include <minecraft/src-client/common/client/renderer/TextureGroup.hpp>
#include <minecraft/src-client/common/client/renderer/screen/MinecraftUIRenderContext.hpp>
#include <minecraft/src-client/common/client/renderer/Tessellator.hpp>
#include <minecraft/src-client/common/client/game/IClientInstance.hpp>
#include <minecraft/src-client/common/client/gui/gui/GuiData.hpp>
#include <random>

static Tessellator* tess;
static mce::MaterialPtr* mUIMaterial = reinterpret_cast<mce::MaterialPtr*>(SlideAddress(0x59BD7E0));
static mce::ClientTexture* clientTexture = nullptr;
static mce::TexturePtr texturePtr;
mce::TextureGroup* textureGroup = nullptr;
bool hasUnloadedTextures = false; 

void _LoadFontTexture(MinecraftUIRenderContext& ctx) {
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height, bytesPerPixel;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytesPerPixel);

	mce::Blob blob(pixels, width * height * bytesPerPixel);
	cg::ImageDescription description(width, height, mce::TextureFormat::R8G8B8A8_UNORM, cg::ColorSpace::sRGB, cg::ImageType::Texture2D, 1);
	cg::ImageBuffer imageBuffer(std::move(blob), std::move(description));

	ResourceLocation resource("imgui_font");
	textureGroup->uploadTexture(resource, imageBuffer);
	texturePtr = ctx.getTexture(resource, false);
	io.Fonts->TexID = (void*)&texturePtr;
}

IMGUI_IMPL_API bool ImGui_ImplMc_Init(MinecraftUIRenderContext& ctx)
{
	tess = &ctx.mScreenContext->tessellator;

	// Seems to help with subpixel rendering causing pixels of 1px wide lines to disappear
	ImGui::GetStyle().AntiAliasedLines = false;
	ImGui::GetStyle().AntiAliasedFill = false;

	_LoadFontTexture(ctx);

	return true;
}

IMGUI_IMPL_API void ImGui_ImplMc_Shutdown()
{
	tess = nullptr;
}

IMGUI_IMPL_API void ImGui_ImplMc_NewFrame(MinecraftUIRenderContext& context)
{
	ImGuiIO& io = ImGui::GetIO();
	GuiData* guiData = context.mClient->guiData;

	io.DisplaySize.x = guiData->totalScreenSize.x;
	io.DisplaySize.y = guiData->totalScreenSize.y;
}

int i = 0;

IMGUI_IMPL_API void ImGui_ImplMc_RenderDrawData(ImDrawData* drawData, MinecraftUIRenderContext& context)
{
	i += 1;

	if (hasUnloadedTextures || i > 500) {
		_LoadFontTexture(context);
		i = 0;
		hasUnloadedTextures = false;
	}
 
	float scale = context.mClient->guiData->mGuiScale;

	for (int n = 0; n < drawData->CmdListsCount; n++) {
		const ImDrawList* cmdList = drawData->CmdLists[n];
		
		for (int cmdIndex = 0; cmdIndex < cmdList->CmdBuffer.size(); cmdIndex++) {
			tess->begin(mce::PrimitiveMode::TriangleList, 0);

			const ImDrawCmd& cmd = cmdList->CmdBuffer[cmdIndex];
			const ImDrawVert* vertices = cmdList->VtxBuffer.Data + cmd.VtxOffset;
			const ImDrawIdx* indices = cmdList->IdxBuffer.Data + cmd.IdxOffset;

			for (uint32_t i = 0; i < cmd.ElemCount; i += 3) {
				// Use 0, 2, 1 for a CW render order
				const ImDrawVert& v0 = vertices[indices[i + 2]];
				const ImDrawVert& v1 = vertices[indices[i + 1]];
				const ImDrawVert& v2 = vertices[indices[i + 0]];

				tess->color(v0.col);
				tess->vertexUV(v0.pos.x / scale, v0.pos.y / scale, 0.0f, v0.uv.x, v0.uv.y);

				tess->color(v1.col);
				tess->vertexUV(v1.pos.x / scale, v1.pos.y / scale, 0.0f, v1.uv.x, v1.uv.y);

				tess->color(v2.col);
				tess->vertexUV(v2.pos.x / scale, v2.pos.y / scale, 0.0f, v2.uv.x, v2.uv.y);
			}

			context.saveCurrentClippingRectangle();
			RectangleArea rect{ cmd.ClipRect.x / scale, cmd.ClipRect.z / scale, cmd.ClipRect.y / scale, cmd.ClipRect.w / scale };
			context.setClippingRectangle(rect);

			mce::Mesh mesh = tess->end(0, "imgui", 0);
			mesh.renderMesh(*context.mScreenContext, *mUIMaterial, texturePtr);

			context.restoreSavedClippingRectangle();
		}
	}
}

void ImGui_ImplMc_OnUnloadAllTextures()
{
	hasUnloadedTextures = true;
}
