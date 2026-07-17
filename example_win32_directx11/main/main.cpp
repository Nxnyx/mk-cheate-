
#include "main.h"
#include <dwmapi.h>
#include "bind.h"
#include "auth.hpp"
#include <sstream>

static bool animated_background = false;
static ID3D11ShaderResourceView* dr = nullptr;
static ID3D11ShaderResourceView* dr1 = nullptr;
static RECT rc = { 0 };
static bool open = true;

void Particles()
{
    const ImVec2 screen_size = { static_cast<float>(GetSystemMetrics(SM_CXSCREEN)), static_cast<float>(GetSystemMetrics(SM_CYSCREEN)) };

    static ImVec2 particle_pos[50];
    static ImVec2 particle_target_pos[50];
    static float particle_speed[50];
    static float particle_radius[50];

    for (int i = 0; i < 50; ++i)
    {
        if (particle_pos[i].x == 0.0f || particle_pos[i].y == 0.0f)
        {
            particle_pos[i].x = static_cast<float>(rand() % static_cast<int>(screen_size.x) + 1);
            particle_pos[i].y = 15.0f;
            particle_speed[i] = static_cast<float>(1 + rand() % 25);
            particle_radius[i] = static_cast<float>(rand() % 4);

            particle_target_pos[i].x = static_cast<float>(rand() % static_cast<int>(screen_size.x));
            particle_target_pos[i].y = screen_size.y * 2.0f;
        }

        particle_pos[i] = ImLerp(particle_pos[i], particle_target_pos[i], ImGui::GetIO().DeltaTime * (particle_speed[i] / 60.0f));

        if (particle_pos[i].y > screen_size.y)
        {
            particle_pos[i] = ImVec2(0.0f, 0.0f);
        }

        ImGui::GetBackgroundDrawList()->AddCircleFilled(particle_pos[i], particle_radius[i], ImColor(255, 255, 255, 255));
    }
}
static HWND hwnd = nullptr;
static bool check = true;
void hide()
{
    if (GetAsyncKeyState(var::key4) && check)
    {
        check = false;
        open = !open;
    }
    else if (!GetAsyncKeyState(var::key4))
    {
        check = true;
    }
}

// KeyAuth configuration - Replace with your KeyAuth credentials
std::string app_name = "";
std::string owner_id = "";
std::string path = "";
std::string version = "1.0";
std::string url = "https://keyauth.win/api/1.3/";

KeyAuth::api KeyAuthApp(app_name, owner_id, version, url, path);

static bool login = true;
static int login_tab = 0;
static char login_username[256] = "";
static char login_password[256] = "";
static char register_username[256] = "";
static char register_password[256] = "";
static char register_key[256] = "";
static char register_email[256] = "";
static char login_error_message[512] = "";
void move_window()
{
    GetWindowRect(hwnd, &rc);
    const ImVec2 window_pos = ImGui::GetWindowPos();

    if (window_pos.x != 0.0f || window_pos.y != 0.0f)
    {
        MoveWindow(hwnd, rc.left + static_cast<int>(window_pos.x), rc.top + static_cast<int>(window_pos.y), 855, 650, TRUE);
        ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
    }
}

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    WNDCLASSEXA wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = nullptr;
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = "ImGui";
    wc.lpszClassName = "Example";
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    RegisterClassExA(&wc);
    
    const int screen_width = GetSystemMetrics(SM_CXSCREEN);
    const int screen_height = GetSystemMetrics(SM_CYSCREEN);
    const int window_width = 855;
    const int window_height = 650;
    
    hwnd = CreateWindowExA(0, wc.lpszClassName, "dd", WS_POPUP,
        (screen_width / 2) - (window_width / 2),
        (screen_height / 2) - (window_height / 2),
        window_width, window_height, nullptr, nullptr, nullptr, nullptr);

    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);

    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassA(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    const auto* glyph_ranges = io.Fonts->GetGlyphRangesCyrillic();
    io.Fonts->AddFontFromMemoryTTF(&inter, sizeof(inter), 16.0f * dpi_scale, nullptr, glyph_ranges);
    tab_text1 = io.Fonts->AddFontFromMemoryTTF(&inter, sizeof(inter), 12.0f * dpi_scale, nullptr, glyph_ranges);
    tab_text2 = io.Fonts->AddFontFromMemoryTTF(&inter, sizeof(inter), 24.0f * dpi_scale, nullptr, glyph_ranges);
    tab_text3 = io.Fonts->AddFontFromMemoryTTF(&inter, sizeof(inter), 40.0f * dpi_scale, nullptr, glyph_ranges);
    ico = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof(icon), 25.0f * dpi_scale, nullptr, glyph_ranges);
    ico_2 = io.Fonts->AddFontFromMemoryTTF(&Menuicon, sizeof(Menuicon), 20.0f * dpi_scale, nullptr, glyph_ranges);
    ico_subtab = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof(icon), 35.0f * dpi_scale, nullptr, glyph_ranges);
    ico_logo = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof(icon), 31.0f * dpi_scale, nullptr, glyph_ranges);
    tab_text = io.Fonts->AddFontFromMemoryTTF(&inter, sizeof(inter), 19.0f * dpi_scale, nullptr, glyph_ranges);
    ico_minimize = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof(icon), 27.0f * dpi_scale, nullptr, glyph_ranges);
    ImGui::StyleColorsDark();

    if (var::debug_console)
    {
        AllocConsole();
        FILE* f;
        freopen_s(&f, "CONOUT$", "w", stdout);
        freopen_s(&f, "CONOUT$", "w", stderr);
        std::cout << "[Debug] Console Allocated" << std::endl;
    }

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }

        hide();

        if (done || GetAsyncKeyState(VK_END))
            break;
        if (var::iteration == 0)
            thread1::threadstart();
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (var::esp)
        {
            ImGui::GetBackgroundDrawList()->AddRect(
                ImVec2((float)var::boxX, (float)var::boxY),
                ImVec2((float)(var::boxX + var::Width), (float)(var::boxY + var::Height)),
                ImColor(255, 0, 0, 255),
                0.0f, 0, 2.0f
            );
        }

        CustomStyleColor();

        D3DX11_IMAGE_LOAD_INFO info;
        ID3DX11ThreadPump* pump = nullptr;

        if (us == nullptr)
            D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, user, sizeof(user), &info, pump, &us, nullptr);

        ImGui::SetNextWindowSize(ImVec2(855.0f * dpi_scale, 650.0f * dpi_scale));
        if (dr1 == nullptr)
            D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, roll, sizeof(roll), &info, pump, &dr1, nullptr);

        if (open)
        {
            if (!login)
            {
                ImGui::Begin("Login", &menu, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
                move_window();
                
                const ImVec2& p = ImGui::GetWindowPos();
                ImGuiStyle& s = ImGui::GetStyle();
                
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0.0f + p.x, 0.0f + p.y), ImVec2(855.0f * dpi_scale + p.x, 650.0f * dpi_scale + p.y), ImGui::GetColorU32(colors::main_color), s.WindowRounding);
                
                ImGui::SetCursorPos(ImVec2(327.5f * dpi_scale, 50.0f * dpi_scale));
                ImGui::BeginChild("LoginWindow", ImVec2(400.0f * dpi_scale, 550.0f * dpi_scale), true);
                {
                    ImGui::SetCursorPos(ImVec2(150.0f * dpi_scale, 20.0f * dpi_scale));
                    ImGui::Text("Authentication");
                    
                    ImGui::SetCursorPos(ImVec2(50.0f * dpi_scale, 60.0f * dpi_scale));
                    if (ImGui::Button("Login", ImVec2(150.0f * dpi_scale, 40.0f * dpi_scale)))
                        login_tab = 0;
                    
                    ImGui::SameLine();
                    if (ImGui::Button("Register", ImVec2(150.0f * dpi_scale, 40.0f * dpi_scale)))
                        login_tab = 1;
                    
                    ImGui::SetCursorPos(ImVec2(20.0f * dpi_scale, 120.0f * dpi_scale));
                    ImGui::BeginChild("LoginContent", ImVec2(360.0f * dpi_scale, 400.0f * dpi_scale), false);
                    {
                        if (login_tab == 0)
                        {
                            ImGui::Text("Username:");
                            ImGui::InputText("##username", login_username, IM_ARRAYSIZE(login_username));
                            
                            ImGui::Spacing();
                            ImGui::Text("Password:");
                            ImGui::InputText("##password", login_password, IM_ARRAYSIZE(login_password), ImGuiInputTextFlags_Password);
                            
                            ImGui::Spacing();
                            if (strlen(login_error_message) > 0)
                            {
                                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), login_error_message);
                            }
                            
                            ImGui::SetCursorPos(ImVec2(100.0f * dpi_scale, 200.0f * dpi_scale));
                            if (ImGui::Button("Login", ImVec2(160.0f * dpi_scale, 40.0f * dpi_scale)))
                            {
                                KeyAuthApp.init();
                                KeyAuthApp.login(login_username, login_password);
                                
                                if (KeyAuthApp.response.success)
                                {
                                    login = true;
                                    strcpy_s(login_error_message, "");
                                }
                                else
                                {
                                    strcpy_s(login_error_message, KeyAuthApp.response.message.c_str());
                                }
                            }
                        }
                        else
                        {
                            ImGui::Text("Username:");
                            ImGui::InputText("##reg_username", register_username, IM_ARRAYSIZE(register_username));
                            
                            ImGui::Spacing();
                            ImGui::Text("Password:");
                            ImGui::InputText("##reg_password", register_password, IM_ARRAYSIZE(register_password), ImGuiInputTextFlags_Password);
                            
                            ImGui::Spacing();
                            ImGui::Text("License Key:");
                            ImGui::InputText("##reg_key", register_key, IM_ARRAYSIZE(register_key));
                            
                            ImGui::Spacing();
                            ImGui::Text("Email (Optional):");
                            ImGui::InputText("##reg_email", register_email, IM_ARRAYSIZE(register_email));
                            
                            ImGui::Spacing();
                            if (strlen(login_error_message) > 0)
                            {
                                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), login_error_message);
                            }
                            
                            ImGui::SetCursorPos(ImVec2(100.0f * dpi_scale, 300.0f * dpi_scale));
                            if (ImGui::Button("Register", ImVec2(160.0f * dpi_scale, 40.0f * dpi_scale)))
                            {
                                KeyAuthApp.init();
                                std::string email_str = strlen(register_email) > 0 ? register_email : "";
                                KeyAuthApp.regstr(register_username, register_password, register_key, email_str);
                                
                                if (KeyAuthApp.response.success)
                                {
                                    KeyAuthApp.login(register_username, register_password);
                                    if (KeyAuthApp.response.success)
                                    {
                                        login = true;
                                        strcpy_s(login_error_message, "");
                                    }
                                    else
                                    {
                                        strcpy_s(login_error_message, "Registration successful but login failed");
                                    }
                                }
                                else
                                {
                                    strcpy_s(login_error_message, KeyAuthApp.response.message.c_str());
                                }
                            }
                        }
                    }
                    ImGui::EndChild();
                }
                ImGui::EndChild();
                ImGui::End();
            }
            else if (login)
            {
                ImGui::Begin("Menu", &menu, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
                move_window();

                const ImVec2& p = ImGui::GetWindowPos();
                ImGuiStyle& s = ImGui::GetStyle();

                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
                ImGui::BeginChild("G-Tab", ImVec2(173.0f * dpi_scale, 790.0f * dpi_scale), false);
                {
                    ImGui::GetForegroundDrawList()->AddText(tab_text3, 20.0f * dpi_scale, ImVec2(20.0f * dpi_scale + p.x, 12.0f * dpi_scale + p.y), ImColor(255, 255, 255, 255), "       Keyvex");
                    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0.0f + p.x, 0.0f + p.y), ImVec2(273.0f * dpi_scale + p.x, 790.0f * dpi_scale + p.y), ImGui::GetColorU32(colors::Tab_Child), s.WindowRounding);

                    ImGui::SetCursorPosY(60.0f);
                    ImGui::SetWindowFontScale(dpi_scale);
                    
                    if (ImGui::Tab("H", "Aimbot", "AI based aimbot", tabs == 0, ImVec2(150.0f * dpi_scale, 42.0f * dpi_scale)))
                        tabs = 0;
                    if (ImGui::Tab("E", "Misc", "Other settings", tabs == 1, ImVec2(150.0f * dpi_scale, 42.0f * dpi_scale)))
                        tabs = 1;
                }
                ImGui::EndChild();
                ImGui::PopStyleColor();

                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0.0f + p.x, 0.0f + p.y), ImVec2(855.0f * dpi_scale + p.x, 790.0f * dpi_scale + p.y), ImGui::GetColorU32(colors::main_color), s.WindowRounding);
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0.0f + p.x, 755.0f * dpi_scale + p.y), ImVec2(855.0f * dpi_scale + p.x, 755.0f * dpi_scale + p.y), ImGui::GetColorU32(colors::lite_color), s.WindowRounding);

                const float delta_time = ImGui::GetIO().DeltaTime;
                tab_alpha = ImClamp(tab_alpha + (7.0f * delta_time * (tabs == active_tab ? 1.0f : -1.0f)), 0.0f, 1.0f);
                tab_add = ImClamp(tab_add + (50.0f * delta_time * (tabs == active_tab ? 1.0f : -1.0f)), 0.0f, 1.0f);

                if (tab_alpha == 0.0f && tab_add == 0.0f)
                    active_tab = tabs;

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, tab_alpha * s.Alpha);
                ImGui::SetCursorPos(ImVec2(203.0f * dpi_scale, 30.0f * dpi_scale));

                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
                ImGui::BeginChild("General", ImVec2(717.0f * dpi_scale, 650.0f * dpi_scale), false);
                {
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetColorU32(colors::lite_color));
                    switch (active_tab)
                    {
                    case 0:
                {
                    ImGui::SetCursorPosY(5.0f * dpi_scale);
                    ImGui::BeginChildPos("ambt", ImVec2(300.0f * dpi_scale, 580.0f * dpi_scale));
                    {
                        ImGui::SetWindowFontScale(dpi_scale);
                        ImGui::Checkbox("Aimbot", &var::checkbox);
                        ImGui::Keybind("Keybind", &var::key0, true);
                        ImGui::SliderFloat("Smoothness", &var::smooth, 5.0f, 100.0f, "%.3f", 0);
                        ImGui::SliderFloat("Aim height", &var::aim_height, 1.0f, 100.0f, "%.3f", 0);
                        ImGui::SliderInt("Scanning frequence", &var::scannFPS, 1, 250, "%d FPS", 0);
                        ImGui::SameLine(270.0f);
                        ImGui::Text("[?]");
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                        {
                            ImGui::SetTooltip("Lowering the AI scanning FPS reduces computer resource usage but may impact aiming accuracy");
                        }
                        ImGui::Checkbox("Show detections", &var::esp);
                        ImGui::SameLine(270.0f);
                        ImGui::Text("[?]");
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                        {
                            ImGui::SetTooltip("Soon");
                        }
                        ImGui::Checkbox("Show FOV", &var::fovCircle);
                        ImGui::SameLine(270.0f);
                        ImGui::Text("[?]");
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                        {
                            ImGui::SetTooltip("Soon");
                        }
                    }
                    ImGui::EndChild();

                    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0.0f + p.x, 0.0f + p.y), ImVec2(855.0f + p.x, 790.0f + p.y), ImGui::GetColorU32(colors::main_color), s.WindowRounding);
                    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0.0f + p.x, 759.0f + p.y), ImVec2(855.0f + p.x, 790.0f + p.y), ImGui::GetColorU32(colors::lite_color), s.WindowRounding);

                    if (dr == nullptr)
                        D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, dragon, sizeof(dragon), &info, pump, &dr, nullptr);
                    
                    ImGui::SetCursorPos(ImVec2(320.0f * dpi_scale, 5.0f * dpi_scale));
                    ImGui::BeginChildPos("Visual Preview", ImVec2(300.0f * dpi_scale, 580.0f * dpi_scale));
                    {
                        ImGui::SetWindowFontScale(dpi_scale);
                        ImGui::SetCursorPosY(135.0f);
                        ImGui::SetCursorPosX(10.0f);
                        ImGui::VSliderFloat(" ", ImVec2(30.0f, 415.0f), &var::aim_height, 0.0f, 100.0f, "", 0);
                        
                        ImVec2 pos = ImGui::GetWindowPos();
                        ImDrawList* draw = ImGui::GetWindowDrawList();
                        draw->AddImageRounded(dr, ImVec2(pos.x - 110.0f, pos.y + 80.0f), ImVec2(pos.x + 410.0f, pos.y + 580.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImColor(255, 255, 255, 255), 10.0f);

                        if (var::esp)
                        {
                            ImGui::SetCursorPos(ImVec2(35.0f, 70.0f));
                            const ImVec2 pos1 = ImGui::GetCursorScreenPos();
                            draw->AddRect(ImVec2(pos1.x, pos1.y), ImVec2(pos1.x + 220.0f, pos1.y + 480.0f), ImColor(255, 255, 255, 255), 0.0f, 0.0f, 1.0f);
                        }
                    }
                    ImGui::EndChild();
                }
                break;
                case 1:
                {
                    ImGui::BeginChildPos("", ImVec2(620.0f * dpi_scale, 100.0f * dpi_scale));
                    {
                        ImGui::GetForegroundDrawList()->AddText(tab_text3, 26.0f * dpi_scale, ImVec2(450.0f * dpi_scale + p.x, 55.0f * dpi_scale + p.y), ImColor(255, 255, 255, 255), "Miscellaneous");
                        ImGui::GetForegroundDrawList()->AddText(tab_text3, 16.0f * dpi_scale, ImVec2(390.0f * dpi_scale + p.x, 85.0f * dpi_scale + p.y), ImColor(255, 255, 255, 255), "Modify menu games and other functions");
                    }
                    ImGui::EndChild();
                    ImGui::SetCursorPosY(120.0f * dpi_scale);
                    ImGui::BeginChildPos("Misc", ImVec2(620.0f * dpi_scale, 490.0f * dpi_scale));
                    {
                        ImGui::SetWindowFontScale(dpi_scale);
                        ImGui::Checkbox("Render animated background", &animated_background);
                        ImGui::Keybind("Hide menu", &var::key4, true);
                        
                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Spacing();
                        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 0.8f), "Detection: %s", var::detection_backend.c_str());
                    }
                    ImGui::EndChild();
                }
                break;
                }
                    ImGui::PopStyleColor();
                    ImGui::Spacing();
                    ImGui::EndChild();
                    ImGui::PopStyleColor();
                }

                if (animated_background)
                    Particles();

                ImGui::PopStyleVar();
                
                const ImVec2 window_pos = ImGui::GetWindowPos();
                const ImVec2 window_size = ImGui::GetWindowSize();
                ImGui::GetForegroundDrawList()->AddText(nullptr, 11.0f * dpi_scale, 
                    ImVec2(window_pos.x + window_size.x - 120.0f * dpi_scale, window_pos.y + window_size.y - 20.0f * dpi_scale),
                    ImColor(100, 100, 100, 150), var::detection_backend.c_str());
                
                ImGui::End();
            }
        }

        ImGui::Render();
        const float clear_color_with_alpha[4] = { 0.0f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        g_pSwapChain->Present(1, 0);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassA(wc.lpszClassName, wc.hInstance);
    return 0;
}

bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd = { 0 };
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    D3D_FEATURE_LEVEL featureLevel;
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    
    if (FAILED(res))
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain)
    {
        g_pSwapChain->Release();
        g_pSwapChain = nullptr;
    }
    if (g_pd3dDeviceContext)
    {
        g_pd3dDeviceContext->Release();
        g_pd3dDeviceContext = nullptr;
    }
    if (g_pd3dDevice)
    {
        g_pd3dDevice->Release();
        g_pd3dDevice = nullptr;
    }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView)
    {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
    }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return 1;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = static_cast<UINT>(LOWORD(lParam));
        g_ResizeHeight = static_cast<UINT>(HIWORD(lParam));
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
