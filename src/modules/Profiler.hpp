#pragma once
#include <imgui.h>
#include <implot.h>
#include <imgui_mc/ShiftingBuffer.hpp>
#include <minecraft/src-client/common/client/gui/ScreenView.hpp>

ShiftingBuffer<float> fpsData(2000);

void RenderProfiler(ScreenView& screen) {
    fpsData.addValue(screen.mTickTime * 1000);

    ImGui::Begin("Profiler");

	ImVec2 plotSize = ImGui::GetContentRegionAvail();
	plotSize.y = 150;

	if (ImPlot::BeginPlot("", plotSize, ImPlotFlags_NoLegend | ImPlotFlags_NoInputs | ImPlotFlags_NoFrame)) {
		ImPlot::SetupAxesLimits(0, (double)fpsData.maxSize(), 0, 60.0, ImGuiCond_Always);
		ImPlot::SetupAxes(NULL, "TickTime (MS)");
		ImPlot::SetupAxisTicks(ImAxis_X1, NULL, 0);
		ImPlot::PlotLine("TickTime (MS)", fpsData.data(), (int)fpsData.dataSize());
		ImPlot::EndPlot();
	}

	ImGui::End();
}