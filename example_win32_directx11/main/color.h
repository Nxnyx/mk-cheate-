#pragma once
#include "imgui.h"



namespace colors {

    inline ImVec4 general_color = ImColor(140, 80, 230, 255); // Violet électrique moderne
    inline ImVec4 main_color = ImColor(20, 22, 28, 255);      // Fond sombre moderne
    inline ImVec4 lite_color = ImColor(28, 31, 38, 255);      // Zones enfants légèrement plus claires
    inline ImVec4 gray_color = ImColor(45, 48, 58, 255);
    inline ImVec4 Tab_Child = ImColor(24, 26, 32, 255);
    inline ImVec4 Tab_Border = ImColor(40, 43, 53, 255);

    inline ImVec4 Tab_Selected = ImColor(140, 80, 230, 255);  // Violet électrique
    inline ImVec4 Tab_Hovered = ImColor(160, 100, 250, 255);
    inline ImVec4 Tab = ImColor(130, 135, 145, 255);

    inline ImVec4 Checkbox = ImColor(24, 26, 32, 255);
    inline ImVec4 Checkbox_Hovered = ImColor(32, 35, 44, 255);
    inline ImVec4 Checkbox_Active = ImColor(140, 80, 230, 255);

    inline ImVec4 Car_Slider = ImColor(140, 80, 230, 255);
    inline ImVec4 Car_Slider_Hovered = ImColor(160, 100, 250, 255);
    inline ImVec4 Car_Slider_Active = ImColor(160, 100, 250, 255);

    inline ImVec4 Slider = ImColor(24, 26, 32, 255);
    inline ImVec4 Slider_Hovered = ImColor(32, 35, 44, 255);
    inline ImVec4 Slider_Active = ImColor(32, 35, 44, 255);

    inline ImVec4 Circle_Slider = ImColor(255, 255, 255, 255);
    inline ImVec4 Circle_SliderHovered = ImColor(255, 255, 255, 255);
    inline ImVec4 Circle_SliderActive = ImColor(255, 255, 255, 255);

    inline ImVec4 Combo = ImColor(32, 35, 44, 255);
    inline ImVec4 Combo_Hovered = ImColor(40, 44, 55, 255);
    inline ImVec4 Combo_Active = ImColor(40, 44, 55, 255);

    inline ImVec4 InputText = ImColor(24, 26, 32, 255);
    inline ImVec4 InputText_Hovered = ImColor(32, 35, 44, 255);
    inline ImVec4 InputText_Active = ImColor(32, 35, 44, 255);

    inline ImVec4 Button = ImColor(32, 35, 44, 255);
    inline ImVec4 Button_Hovered = ImColor(40, 44, 55, 255);
    inline ImVec4 Button_Active = ImColor(45, 49, 62, 255);

    inline ImVec4 Selectable_Hovered = ImColor(140, 80, 230, 100);
    inline ImVec4 Selectable_Active = ImColor(140, 80, 230, 255);

    inline ImVec4 Picker_Active = ImColor(28, 31, 38, 255);

    inline ImVec4 Text = ImColor(180, 185, 195, 255);
    inline ImVec4 Text_Hovered = ImColor(230, 235, 245, 255);
    inline ImVec4 Text_Active = ImColor(255, 255, 255, 255);

    inline ImVec4 CheckMark = ImColor(255, 255, 255, 255);

    inline ImVec4 Transparent = ImColor(0, 0, 0, 0);

    inline ImVec4 tab_tooltip = ImColor(200, 205, 215, 255);

    inline ImVec4 keybind_background = ImColor(24, 26, 32, 255);
    inline ImVec4 keybind_border = ImColor(40, 43, 53, 255);
}
