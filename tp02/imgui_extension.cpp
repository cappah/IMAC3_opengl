#include "imgui_extension.h"
//forwards : 
#include "dirent.h"
#include "Utils.h"
#include "FileHandler.h"

namespace ImGui {

	bool MyTreeNode(const char* label, ImVec2& itemPos, ImVec2& itemSize)
	{
		ImGuiState& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		const ImGuiStyle& style = g.Style;

		ImU32 id = window->GetID(label);
		bool opened = ImGui::TreeNodeBehaviorIsOpened(id);

		float circle_radius = 8.f;

		ImVec2 pos = window->DC.CursorPos;
		ImRect bb(pos, ImVec2(pos.x + ImGui::GetContentRegionAvail().x, pos.y + g.FontSize + g.Style.FramePadding.y * 2));
		const ImVec2 padding = opened ? style.FramePadding : ImVec2(style.FramePadding.x, 0.0f);
		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const float collapser_width = style.FramePadding.x + g.FontSize*0.5f + circle_radius;
		const float text_width = g.FontSize + (label_size.x > 0.0f ? label_size.x + padding.x * 2 : 0.0f);   // Include collapser
		const float total_width = collapser_width + text_width;

		bb.Max = ImVec2(pos.x + total_width, bb.Max.y);

		itemPos = pos;
		itemSize = ImVec2(ImGui::GetContentRegionAvail().x - bb.GetWidth(), bb.GetHeight());

		bool hovered, held;
		if (ImGui::ButtonBehavior(bb, id, &hovered, &held, true))
			window->DC.StateStorage->SetInt(id, opened ? 0 : 1);
		if (hovered || held)
			window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(1, 0, 0, 1)/*window->Color(held ? ImGuiCol_HeaderActive : ImGuiCol_HeaderHovered)*/);

		// Icon, text
		float button_sz = g.FontSize + g.Style.FramePadding.y * 2;
		//window->DrawList->AddRectFilled(pos, ImVec2(pos.x + button_sz, pos.y + button_sz), opened ? ImColor(255, 0, 0) : ImColor(0, 255, 0));
		
		ImGuiCol circle_enum_color = hovered ? ImGuiCol_ButtonHovered : (held ? ImGuiCol_ButtonActive : ImGuiCol_Button);

		const float line_height = ImMax(ImMin(window->DC.CurrentLineHeight, g.FontSize + g.Style.FramePadding.y * 2), g.FontSize);
		window->DrawList->AddCircleFilled(pos + ImVec2(style.FramePadding.x + g.FontSize*0.5f, line_height*0.5f), circle_radius, GetColorU32(circle_enum_color));
		RenderCollapseTriangle(pos + ImVec2(3.f,0.f), opened, 1.0f, true);
		ImGui::RenderText(ImVec2(pos.x + button_sz + g.Style.ItemInnerSpacing.x, pos.y + g.Style.FramePadding.y), label);

		ImGui::ItemSize(bb, g.Style.FramePadding.y);
		ImGui::ItemAdd(bb, &id);

		if (opened)
			ImGui::TreePush(label);
		return opened;
	}


	bool ImGui::MyTreeNode2(const char* ptr_id)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiState& g = *GImGui;

		ImGui::PushID(ptr_id);
		const bool opened = ImGui::MyCollapsingHeader(ptr_id, "", false);
		ImGui::PopID();

		if (opened)
			ImGui::TreePush(ptr_id);

		return opened;
	}

	bool ImGui::MyCollapsingHeader(const char* label, const char* str_id, bool display_frame, bool default_open)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiState& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImVec2 padding = display_frame ? style.FramePadding : ImVec2(style.FramePadding.x, 0.0f);

		IM_ASSERT(str_id != NULL || label != NULL);
		if (str_id == NULL)
			str_id = label;
		if (label == NULL)
			label = str_id;
		const bool label_hide_text_after_double_hash = (label == str_id); // Only search and hide text after ## if we have passed label and ID separately, otherwise allow "##" within format string.
		const ImGuiID id = window->GetID(str_id);
		const ImVec2 label_size = CalcTextSize(label, NULL, label_hide_text_after_double_hash);

		const float text_base_offset_y = ImMax(0.0f, window->DC.CurrentLineTextBaseOffset - padding.y); // Latch before ItemSize changes it
		const float frame_height = ImMax(ImMin(window->DC.CurrentLineHeight, g.FontSize + g.Style.FramePadding.y * 2), label_size.y + padding.y * 2);
		/////////
		const float collapser_width = g.FontSize + (display_frame ? padding.x * 2 : padding.x);
		const float text_width = g.FontSize + (label_size.x > 0.0f ? label_size.x + padding.x * 2 : 0.0f);   // Include collapser
		ItemSize(ImVec2(text_width, frame_height), text_base_offset_y);
		/////////
		ImRect bb = ImRect(window->DC.CursorPos, ImVec2(window->Pos.x + 50/*GetContentRegionMax().x*/, window->DC.CursorPos.y + frame_height));
		if (display_frame)
		{
			// Framed header expand a little outside the default padding
			bb.Min.x -= (float)(int)(window->WindowPadding.x*0.5f) - 1;
			bb.Max.x += (float)(int)(window->WindowPadding.x*0.5f) - 1;
		}

		// For regular tree nodes, we arbitrary allow to click past 2 worth of ItemSpacing
		// (Ideally we'd want to add a flag for the user to specify we want want the hit test to be done up to the right side of the content or not)
		const ImRect interact_bb = display_frame ? bb : ImRect(bb.Min.x, bb.Min.y, bb.Min.x + text_width + style.ItemSpacing.x * 2, bb.Max.y);
		bb = interact_bb;
		bool opened = TreeNodeBehaviorIsOpened(id, (default_open ? ImGuiTreeNodeFlags_DefaultOpen : 0) | (display_frame ? ImGuiTreeNodeFlags_NoAutoExpandOnLog : 0));
		if (!ItemAdd(interact_bb, &id))
			return opened;

		bool hovered, held;
		bool pressed = ButtonBehavior(interact_bb, id, &hovered, &held, ImGuiButtonFlags_NoKeyModifiers);
		if (pressed)
		{
			opened = !opened;
			window->DC.StateStorage->SetInt(id, opened);
		}

		// Render
		const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
		const ImVec2 text_pos = bb.Min + padding + ImVec2(collapser_width, text_base_offset_y);
		if (display_frame)
		{
			// Framed type
			RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);
			RenderCollapseTriangle(bb.Min + padding + ImVec2(0.0f, text_base_offset_y), opened, 1.0f, true);

				RenderTextClipped(text_pos, bb.Max, label, NULL, &label_size);
		}
		else
		{
			// Unframed typed for tree nodes
			if (hovered)
				RenderFrame(bb.Min, bb.Max, col, false);

			RenderCollapseTriangle(bb.Min + ImVec2(padding.x, g.FontSize*0.15f + text_base_offset_y), opened, 0.70f, false);

			RenderText(text_pos, label, NULL, label_hide_text_after_double_hash);
		}

		return opened;
	}

	bool InputFilePath(const char* label, char* entry, size_t stringSize)
	{
		bool shouldCloseAutocompletionWindow = false;
		bool resetItemSelected = false;
		float posAutoCompletionX = ImGui::GetCursorPosX() + ImGui::GetWindowPos().x;
		
		
		bool textEntered = ImGui::InputText(label, entry, stringSize, 0&(ImGuiInputTextFlags_AutoSelectAll));
		if (textEntered) {
			AutoCompletion::get().setIsActive(true);
			AutoCompletion::get().setHasFocus(false);
		}

		if (!ImGui::IsItemActive()) {
			if (!AutoCompletion::get().getItemSelected())
			{
				shouldCloseAutocompletionWindow = true;
				AutoCompletion::get().setIsActive(false);
			}
		}

		if (AutoCompletion::get().getItemSelected()) {
			ImGui::SetKeyboardFocusHere(-1);
			resetItemSelected = true;
		}

		float posAutoCompletionY = ImGui::GetCursorPosY() + ImGui::GetWindowPos().y;

		ImGui::SetNextWindowPos(ImVec2(posAutoCompletionX, posAutoCompletionY));
		if (AutoCompletion::get().getIsOpen())
		{
			ImGui::BeginTooltip();
			
			std::string path;
			std::string filename;
			std::size_t splitLocation = FileHandler::splitPathFileName(entry, path, filename);
			if (path == "")
				path = "." + path;
			else
				path += "/";

			std::vector<std::string> fileAndDirNames = FileHandler::getAllFileAndDirNames( path );
			AutoCompletion::get().clearWords();

			for (auto& name : fileAndDirNames) {
				if (name.compare(0, filename.size(), filename)==0 || filename.size() == 0) {
					AutoCompletion::get().addWord(name);
				}
			}

			bool autocompletionApplied = AutoCompletion::get().apply(filename);
			if (autocompletionApplied)
			{
				if (path[0] == '.')
					entry[splitLocation] = '\0';
				else {
					entry[splitLocation] = '/';
					entry[splitLocation+1] = '\0';
				}
				std::strcat(entry, filename.c_str());
			}

			AutoCompletion::get().setIsOpen(false);
			ImGui::EndTooltip();
		}

		AutoCompletion::get().setIsOpen(!shouldCloseAutocompletionWindow);
		if(resetItemSelected)
			AutoCompletion::get().setItemSelected(false);

		return textEntered;

	}

	bool IsMouseClickedAnyButton()
	{
		ImGuiState& g = *GImGui;
		int arraySize = IM_ARRAYSIZE(g.IO.MouseDown);
		for (int i = 0; i < arraySize; i++)
		{
			if (IsMouseClicked(i))
				return true;
		}
		return false;
	}
}

