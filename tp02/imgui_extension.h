#pragma once
#include <cmath>
#include <algorithm>
#include "imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"


namespace ImGui {

	bool MyTreeNode(const char* label, ImVec2& itemPos, ImVec2& itemSize);
	bool MyTreeNode2(const char* ptr_id);
	bool MyCollapsingHeader(const char* label, const char* str_id = NULL, bool display_frame = true, bool default_open = false);
	bool InputFilePath(const char* label, char* entry, size_t stringSize);
	bool IsMouseClickedAnyButton();
}
