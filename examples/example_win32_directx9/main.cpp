#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include "imgui_internal.h"
#include "logo.h"
#include "byte.h"
#include <d3dx9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include <vector>
#include <functional>
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

float dpi_scale = 1.f;

#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 400

ImFont* info = nullptr;
ImFont* iconfont = nullptr;
ImFont* backfont = nullptr;
ImFont* loadfont = nullptr;
ImFont* iconfont_big = nullptr;
ImFont* info_little = nullptr;
ImVec2 pos;
ImDrawList* draw;



// Data
IDirect3DTexture9* logggo = nullptr;
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};
// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static int registration = 0;
static int tab = 0;

void include()
{
    pos = ImGui::GetWindowPos();
    draw = ImGui::GetWindowDrawList();
};

int main(int, char**)
{

    RECT rc;
    POINT mouse;
    GetWindowRect(GetDesktopWindow(), &rc);
    rc.left = 0;
    rc.top = 0;
    rc.right = GetSystemMetrics(SM_CXMAXTRACK);    //If you have more than one monitor, you will get the total width
    rc.bottom = GetSystemMetrics(SM_CYMAXTRACK);

    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, /*LoadCursor(NULL, IDC_ARROW)*/ NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = CreateWindowEx(NULL, _T("ImGui Example"), NULL, WS_POPUP, (GetSystemMetrics(SM_CXSCREEN) / 2) - (WINDOW_WIDTH / 2), (GetSystemMetrics(SM_CYSCREEN) / 2) - (WINDOW_HEIGHT / 2), WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, 0, 0);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);


    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGuiStyle* style = &ImGui::GetStyle();

    ImGui::StyleColorsDark();

    style->Alpha = 1.f;
    style->WindowRounding = 12.f;
    style->FramePadding = ImVec2(4, 3);
    style->WindowPadding = ImVec2(8, 8);
    style->ItemInnerSpacing = ImVec2(4, 4);
    style->ItemSpacing = ImVec2(8, 15);
    style->FrameRounding = 4.f;
    style->ScrollbarSize = 2.f;
    style->ScrollbarRounding = 12.f;
    style->PopupRounding = 4.f;


    ImVec4* colors = ImGui::GetStyle().Colors;

    colors[ImGuiCol_ChildBg] = ImVec4(12, 13, 13, 0.1f);
    colors[ImGuiCol_Border] = ImVec4(255, 255, 255, 0);
    colors[ImGuiCol_FrameBg] = ImColor(53, 53, 53);
    colors[ImGuiCol_FrameBgActive] = ImColor(25, 25, 33, 255);
    colors[ImGuiCol_FrameBgHovered] = ImColor(25, 25, 33, 255);
    colors[ImGuiCol_Header] = ImColor(21, 21, 20, 0);
    colors[ImGuiCol_HeaderHovered] = ImColor(21, 21, 20, 0);
    colors[ImGuiCol_HeaderActive] = ImColor(21, 21, 20, 0);
    colors[ImGuiCol_PopupBg] = ImColor(54, 54, 54);
    colors[ImGuiCol_Button] = ImColor(54, 54, 54);
    colors[ImGuiCol_ButtonHovered] = ImColor(41, 41, 41);
    colors[ImGuiCol_ButtonActive] = ImColor(54,54,54);
    colors[ImGuiCol_PlotHistogram] = ImColor(147, 149, 151);

    ImFontConfig font_config;
    font_config.OversampleH = 1; //or 2 is the same
    font_config.OversampleV = 1;
    font_config.PixelSnapH = 1;

    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0400, 0x044F, // Cyrillic
        0,
    };


    info = io.Fonts->AddFontFromFileTTF("C:/windows/fonts/arial.ttf", 14);
    info_little = io.Fonts->AddFontFromFileTTF("C:/windows/fonts/arial.ttf", 11);
    iconfont = io.Fonts->AddFontFromMemoryTTF((void*)icon, sizeof(icon), 25, &font_config, ranges);
    iconfont_big = io.Fonts->AddFontFromMemoryTTF((void*)icon, sizeof(icon), 70, &font_config, ranges);
    backfont = io.Fonts->AddFontFromMemoryTTF((void*)neofont, sizeof(neofont), 25, &font_config, ranges);
    loadfont = io.Fonts->AddFontFromMemoryTTF((void*)neofont, sizeof(neofont), 95, &font_config, ranges);

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    bool loginpanel = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.00f);
    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {

        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }
        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("Neo ImGui Design", &loginpanel, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus);
            {
                GetWindowRect(hwnd, &rc);

                if (ImGui::GetWindowPos().x != 0 || ImGui::GetWindowPos().y != 0)
                {
                    MoveWindow(hwnd, rc.left + ImGui::GetWindowPos().x, rc.top + ImGui::GetWindowPos().y, WINDOW_WIDTH, WINDOW_HEIGHT, TRUE);
                    ImGui::SetWindowPos(ImVec2(0.f, 0.f));
                }

                GetCursorPos(&mouse);

                ImGui::SetWindowSize(ImVec2(600, 400));
                include();

                ImVec2 screenSize = ImGui::GetIO().DisplaySize;
                ImGui::Particles(draw, screenSize);
                if (Settings::Tab == 0)
                {

                    if (logggo == nullptr)
                        D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, &logo, sizeof(logo), 250, 225, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &logggo);

                    ImGui::SetCursorPos(ImVec2(170, 0));
                    ImGui::Image(logggo, ImVec2(260, 225));

                    static bool fill = false;
                    bool show_another_window = false;
                    static bool animate = true;
                    static bool exit = false;
                    static bool steamgroup = false;
                    static bool registerserver = false;

                    ImGui::PushFont(backfont);

                    ImGui::SetCursorPos(ImVec2(570, 15));
                    ImGui::Selectable("A", &exit, 0, ImVec2(25, 25));

                    ImGui::SetCursorPos(ImVec2(567, 365));
                    ImGui::Selectable("C", &steamgroup, 0, ImVec2(25, 25));

                    if (steamgroup)
                    {
                        static float progress = 0.0f, progress_dir = 1.0f;
                        if (animate)
                        {
                            progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
                            if (progress >= +1.1f) { progress = +1.1f; }
                            if (progress <= -0.1f) { progress = -0.1f; }
                        }

                        float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
                        char buf[32];
                        sprintf_s(buf, "%d/%d", (int)(progress_saturated * 1753), 1753);

                        if (progress <= 0.01f)
                        {
                            ShellExecute(0, 0, L"http://steamcommunity.com/groups/neoimguidesign", 0, 0, SW_SHOW);
                        }
                    }


                    ImGui::PopFont();

                    if (exit)
                    {
                        DestroyWindow(hwnd);
                    }

                    ImGui::SetCursorPos(ImVec2(137, 190));
                    ImGui::TextColored(ImColor(212, 212, 211), "Username");
                    ImGui::SetCursorPos(ImVec2(137, 205));
                    ImGui::PushItemWidth(325);
                    static char buf1[64] = ""; ImGui::InputText("##username", buf1, 64);
                    ImGui::PopItemWidth();
                    ImGui::SetCursorPos(ImVec2(137, 235));
                    ImGui::TextColored(ImColor(212, 212, 211), "Password");

                    ImGui::SetCursorPos(ImVec2(335, 232));
                    ImGui::TextDisabled("Forgot your password?");
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                        ImGui::TextUnformatted("Contact exitfrommatrix#1518 for more information!");
                        ImGui::PopTextWrapPos();
                        ImGui::EndTooltip();
                    }

                    ImGui::SetCursorPos(ImVec2(137, 250));
                    static char bufpass[64] = "";
                    ImGui::PushItemWidth(325);
                    ImGui::InputText("##password", bufpass, 64, ImGuiInputTextFlags_Password | ImGuiInputTextFlags_CharsNoBlank);
                    ImGui::PopItemWidth();

                    ImGui::SetCursorPos(ImVec2(137, 285));
                    static int clicked = 0;
                    static int registerbutton = 0;
                    if (ImGui::Button("Login", ImVec2(120, 30)))
                        clicked++;

                    ImGui::SetCursorPos(ImVec2(170, 380));
                    ImGui::TextDisabled("Don't have an account?");
                    ImGui::SetCursorPos(ImVec2(303, 380));
                    ImGui::Selectable("Join the Discord server!", &registerserver);
                    if (registerserver)
                    {
                        static float progress = 0.0f, progress_dir = 1.0f;
                        if (animate)
                        {
                            progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
                            if (progress >= +1.1f) { progress = +1.1f; }
                            if (progress <= -0.1f) { progress = -0.1f; }
                        }

                        float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
                        char buf[32];
                        sprintf_s(buf, "%d/%d", (int)(progress_saturated * 1753), 1753);

                        if (progress <= 0.01f)
                        {
                            ShellExecute(0, 0, L"https://discord.gg/3gMTau3suH", 0, 0, SW_SHOW);
                        }
                    }

                    if (clicked & 1)
                    {
                        // Animate a simple progress bar
                        static float progress = 0.0f, progress_dir = 1.0f;
                        if (animate)
                        {
                            progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
                            if (progress >= +1.1f) { progress = +1.1f; }
                            if (progress <= -0.1f) { progress = -0.1f; }
                        }

                        ImGui::SetCursorPos(ImVec2(270, 285));
                        ImGui::ProgressBar(progress, ImVec2(190, 30));

                        float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
                        char buf[32];
                        sprintf_s(buf, "%d/%d", (int)(progress_saturated * 1753), 1753);

                        if (progress <= 0.6f)
                        {
                            ImGui::SetCursorPos(ImVec2(137, 320));
                            ImGui::TextColored(ImColor(212, 212, 211), "Attempting to connect to the server...");
                        }


                        if (progress >= 0.6f)
                        {
                            ImGui::SetCursorPos(ImVec2(137, 320));
                            ImGui::TextColored(ImColor(212, 212, 211), "Loading projects...");
                        }

                        if (progress >= 1.1f)
                        {
                            Settings::Tab = 1;
                        }
                    }
                }


                else if (Settings::Tab == 1)
                {
                ImVec2 screenSize = ImGui::GetIO().DisplaySize;

                    if (logggo == nullptr)
                        D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, &logo, sizeof(logo), 450, 220, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &logggo);

                    ImGui::SetCursorPos(ImVec2(210, -10));
                    ImGui::Image(logggo, ImVec2(180, 150));

                    static bool project1 = false;
                    static bool project2 = false;
                    static bool project3 = false;
                    static bool animate = true;
                    static bool exit = false;

                    ImGui::PushFont(backfont);

                    ImGui::SetCursorPos(ImVec2(570, 15));
                    ImGui::Selectable("A", &exit, 0, ImVec2(25, 25));
                    ImGui::PopFont();

                    if (exit)
                    {
                        DestroyWindow(hwnd);
                    }

                    ImGui::SetCursorPos(ImVec2(10, 120));

                    ImGui::BeginChild("##projectchild", ImVec2(300, 265));
                    {
                        ImGui::SetCursorPos(ImVec2(0, 5));

                        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.08f, 0.08f, 0.08f, 1.00f));
                        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.08f, 0.08f, 0.08f, 1.00f));
                        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.22f, 0.22f, 0.21f, 1.00f));
                        ImGui::Selectable(" Project 1 (Undetected)", &project1, 0, ImVec2(300,15));

                        if (project1)
                        {
                            project2 = false;
                            project3 = false;


                        }

                        ImGui::Selectable(" Project 2 (Maintenance)", &project2, 0, ImVec2(300, 15));

                        if (project2)
                        {
                            project1 = false;
                            project3 = false;
                        }

                        ImGui::Selectable(" Project 3 (Detected)", &project3, 0, ImVec2(300, 15));


                        if (project3)
                        {
                            project1 = false;
                            project2 = false;
                        }


                        ImGui::PopStyleColor();
                        ImGui::PopStyleColor();
                        ImGui::PopStyleColor();

                    }
                    ImGui::EndChild();

                    ImGui::SetCursorPos(ImVec2(325, 120));

                    ImGui::BeginChild("##statuschild", ImVec2(260, 150));
                    {
                        ImGui::SetCursorPos(ImVec2(0, 5));

                        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.08f, 0.08f, 0.08f, 1.00f));
                        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.08f, 0.08f, 0.08f, 1.00f));
                        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.22f, 0.22f, 0.21f, 1.00f));

                        ImGui::SetCursorPos(ImVec2(5, 8));
                        ImGui::TextColored(ImColor(212, 212, 211), "Welcome back!");
                        ImGui::SetCursorPos(ImVec2(5, 25));
                        ImGui::TextColored(ImColor(212, 212, 211), "Status:");

                        if (project3)
                        {
                            ImGui::SetCursorPos(ImVec2(45, 25));
                            ImGui::TextColored(ImVec4(0.87f, 0.01f, 0.01f, 1.00f), "Detected");
                            ImGui::SetCursorPos(ImVec2(95, 42));
                            ImGui::TextDisabled("1.0");
                        }
                        if (project1)
                        {
                            ImGui::SetCursorPos(ImVec2(45, 25));
                            ImGui::TextColored(ImVec4(0.00f, 1.00f, 0.15f, 1.00f), "Undetected");
                            ImGui::SetCursorPos(ImVec2(95, 42));
                            ImGui::TextDisabled("1.1");
                        }
                        if (project2)
                        {
                            ImGui::SetCursorPos(ImVec2(45, 25));
                            ImGui::TextColored(ImColor(224, 177, 12), "Maintenance");
                            ImGui::SetCursorPos(ImVec2(95, 42));
                            ImGui::TextDisabled("BETA");
                        }

                        ImGui::SetCursorPos(ImVec2(5, 42));
                        ImGui::TextColored(ImColor(212, 212, 211), "Current version:");

                        ImGui::PopStyleColor();
                        ImGui::PopStyleColor();
                        ImGui::PopStyleColor();

                    }
                    ImGui::EndChild();

                    ImGui::SetCursorPos(ImVec2(325, 280));

                    ImGui::BeginChild("##loadingchild", ImVec2(260, 105));
                    {
                        ImGui::SetCursorPos(ImVec2(0, 5));

                        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.08f, 0.08f, 0.08f, 1.00f));
                        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.08f, 0.08f, 0.08f, 1.00f));
                        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.22f, 0.22f, 0.21f, 1.00f));

                        ImGui::SetCursorPos(ImVec2(33, 37));
                        static int clicked = 0;
                          if (ImGui::Button("Load", ImVec2(195, 25)))
                           clicked++;

                        if (clicked & 1)
                        {
                            Settings::Tab = 2;
                        }

                        ImGui::PopStyleColor();
                        ImGui::PopStyleColor();
                        ImGui::PopStyleColor();

                    }
                    ImGui::EndChild();
                }

                else if (Settings::Tab == 2)
                {
                if (logggo == nullptr)
                    D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, &logo, sizeof(logo), 300, 270, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &logggo);

                ImGui::SetCursorPos(ImVec2(140, -10));
                ImGui::Image(logggo, ImVec2(300, 270));

                static bool fill = false;
                bool show_another_window = false;
                static bool animate = true;
                static bool exit = false;

                ImGui::PushFont(backfont);

                ImGui::SetCursorPos(ImVec2(570, 7));
                ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(62, 70, 55, 0));
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(62, 70, 55, 0));
                ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(62, 70, 55, 0));
                ImGui::Selectable("A", &exit, 0, ImVec2(25, 25));
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::PopFont();

                if (exit)
                {
                    DestroyWindow(hwnd);
                }

                // Animate a simple progress bar
                static float progress = 0.0f, progress_dir = 1.0f;
                if (animate)
                {
                    progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
                    if (progress >= +1.1f) { progress = +1.1f; }
                    if (progress <= -0.1f) { progress = -0.1f; }
                }

                // Typically we would use ImVec2(-1.0f,0.0f) to use all available width, or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
                ImGui::SetCursorPos(ImVec2(98, 230));
                ImGui::ProgressBar(progress, ImVec2(400, 25));

                float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
                char buf[32];
                sprintf_s(buf, "%d/%d", (int)(progress_saturated * 1753), 1753);

                if (progress <= 1.0f)
                {
                    ImGui::SetCursorPos(ImVec2(98, 260));
                    ImGui::TextColored(ImColor(212, 212, 211), "Checking Project...");
                }


                if (progress >= 1.0f)
                {
                    ImGui::SetCursorPos(ImVec2(98, 260));
                    ImGui::TextColored(ImColor(212, 212, 211), "Completed! You can close the application.");
                }
                }



                ImGui::End();
            }
        }

        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;


    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

