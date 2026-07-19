#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <vector>
#include <mutex>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

// Data
inline static ID3D11Device* g_pd3dDevice = nullptr;
inline static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
inline static IDXGISwapChain* g_pSwapChain = nullptr;
inline static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
inline static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include "imgui_internal.h"

#include <D3DX11tex.h>
#pragma comment(lib, "D3DX11.lib")

#include "user_circle.h"
#include "inter.h"
#include "ico_pack.h"

#include "color.h"


inline ImFont* ico = nullptr;
inline ImFont* ico_2 = nullptr;
inline ImFont* ico_minimize = nullptr;
inline ImFont* tab_text = nullptr;
inline ImFont* tab_text1 = nullptr;
inline ImFont* tab_text2 = nullptr;
inline ImFont* tab_text3 = nullptr;
inline ImFont* ico_logo = nullptr;
inline ImFont* ico_subtab = nullptr;
inline ID3D11ShaderResourceView* us = nullptr;

namespace thread1
{
   void threadstart();
   void POC();
  
}
namespace var {

   inline float frequency_factor_x = 0.1; // Ajustez le facteur de frquence selon vos prfrences pour le mouvement en x
    inline float frequency_factor_y = 0.1; // Ajustez le facteur de frquence selon vos prfrences pour le mouvement en y



    inline float randomness_factor = 0.02;
    inline float kp = 0.2, ki = 0.005, kd = 0.02;
    inline bool RTrigger;
    inline bool LTrigger ;
    inline float smoothing_factor = 0.05;
    inline float x_smooth = 0, y_smooth = 0;
   inline int iteration = 0;
  inline float jitter_factor = 1;
   inline int boxY;
   inline int boxX;
   inline int Width;
   inline int Height;
    inline bool fovCircle = false;
    inline float fovSize = 120.0f;
    inline bool checkbox = true;
    inline bool esp = false;
    inline int scannFPS = 100;
    inline float smooth = 30.0f;
    inline float aim_height = 50.0f;
    inline int key0 = VK_RBUTTON;
    inline int key4 = VK_INSERT;
    inline std::string detection_backend = "CPU";
    inline bool debug_console = false;

    // Aim Assist
    inline bool aim_assist_enabled = true;
    inline float aaMovementAmp = 0.35f;
    inline int aimAnchor = 0; // 0: Head, 1: Neck, 2: Body
    inline float confidenceThreshold = 0.60f;
    inline bool target_persistence = true;
    inline bool central_mask = false;
    inline int stop_key = VK_END;

    // UI Offsets
    inline float offsetX = 0.0f;
    inline float offsetY = 0.0f;
    inline float scale = 1.0f;

    // Visual ESP
    inline bool esp_boxes = true;
    inline bool esp_labels = true;
    inline bool esp_snaplines = false;
    inline bool esp_fov_ring = true;
    inline bool esp_status_hud = true;
    inline float esp_color[4] = { 1.0f, 0.0f, 0.0f, 1.0f }; // Red

    // Skeleton Overlay
    inline bool skeleton_enabled = true;
    inline int skeleton_mode = 0; // 0: Box approximation, 1: Pose estimation (simulated)
    inline float skeleton_thickness = 2.0f;
    inline float skeleton_opacity = 1.0f;
    inline float min_joint_confidence = 0.50f;
    inline bool skeleton_active_only = false;

    // Thread safety
    struct Detection {
        float x;
        float y;
        float width;
        float height;
        float confidence;
        bool is_target;
    };
    inline std::vector<Detection> detections;
    inline std::mutex detections_mutex;
}

inline static float tab_alpha = 0.0f;
inline static float tab_add = 0.0f;
inline static int active_tab = 0;
inline int tabs = 0;

inline bool menu = true;
inline float dpi_scale = 1.0f;

inline void CustomStyleColor()
{
    ImGuiStyle& s = ImGui::GetStyle();

    s.Colors[ImGuiCol_WindowBg] = ImColor(20, 22, 28, 240); // Fond sombre moderne
    s.Colors[ImGuiCol_ChildBg] = ImColor(28, 31, 38, 255);  // Zones enfants légèrement plus claires
    s.Colors[ImGuiCol_PopupBg] = ImColor(26, 26, 26, 255);
    s.Colors[ImGuiCol_Text] = ImColor(230, 235, 240, 255);  // Texte blanc doux très lisible
    s.Colors[ImGuiCol_TextDisabled] = ImColor(130, 135, 145, 255);
    s.Colors[ImGuiCol_Border] = ImColor(45, 48, 58, 255);
    s.Colors[ImGuiCol_TextSelectedBg] = ImColor(115, 65, 215, 100);

    s.Colors[ImGuiCol_ScrollbarGrab] = ImColor(35, 35, 45, 255);
    s.Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(45, 45, 55, 255);
    s.Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(55, 55, 65, 255);

    s.WindowBorderSize = 0;
    s.WindowPadding = ImVec2(15.0f, 15.0f); // Espaces aérés
    s.WindowRounding = 12.0f;               // Angles adoucis
    s.PopupBorderSize = 0.f;
    s.PopupRounding = 10.0f;                // Angles adoucis
    s.ChildRounding = 10.0f;                // Angles adoucis
    s.ChildBorderSize = 1.f;
    s.FrameBorderSize = 1.0f;
    s.ScrollbarSize = 4.0f;
    s.FrameRounding = 8.0f;                 // Angles adoucis
    s.GrabRounding = 8.0f;                  // Angles adoucis
    s.ItemSpacing = ImVec2(12.0f, 12.0f);   // Espaces aérés
    s.ItemInnerSpacing = ImVec2(10.0f, 8.0f);
    s.FramePadding = ImVec2(10.0f, 8.0f);   // Espaces aérés
}
