// main.cpp
#include <windows.h>
#include <d3d11.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <string>

// Для простоты скинов и оружия
struct Skin {
    std::string name;
    std::string rarity;
};

struct Weapon {
    std::string name;
    Skin skins[2];
};

// Данные для отображения
Weapon weapons[] = {
    {"AK-47", {{"Redline", "Classified"}, {"Vulcan", "Classified"}}},
    {"AWP", {{"Dragon Lore", "Covert"}, {"Gungnir", "Covert"}}},
    {"Knife", {{"Fade", "Covert"}, {"Marble Fade", "Covert"}}}
};

static int selectedWeapon = 0;
static int selectedSkin = 0;
static bool showOverlayInOBS = false;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    // Регистрация окна
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0, 0,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      L"CS2SkinChanger", NULL };
    RegisterClassEx(&wc);
    HWND hwnd = CreateWindow(wc.lpszClassName, L"CS2 SkinChanger", WS_OVERLAPPEDWINDOW,
                             100, 100, 600, 400, NULL, NULL, wc.hInstance, NULL);

    // Инициализация DirectX11
    D3D_FEATURE_LEVEL featureLevel;
    ID3D11Device* device;
    ID3D11DeviceContext* context;
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = 600;
    sd.BufferDesc.Height = 400;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    IDXGISwapChain* swapChain;
    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
                                  NULL, 0, D3D11_SDK_VERSION, &sd, &swapChain,
                                  &device, &featureLevel, &context);

    ID3D11RenderTargetView* renderTargetView;
    ID3D11Texture2D* backBuffer;
    swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);
    backBuffer->Release();

    // Инициализация ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device, context);

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // Главный цикл
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Наш GUI
        ImGui::Begin("CS2 SkinChanger");

        ImGui::Text("Выбери оружие:");
        if (ImGui::BeginCombo("##weapon", weapons[selectedWeapon].name.c_str()))
        {
            for (int n = 0; n < IM_ARRAYSIZE(weapons); n++)
            {
                bool is_selected = (selectedWeapon == n);
                if (ImGui::Selectable(weapons[n].name.c_str(), is_selected))
                    selectedWeapon = n;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Text("Выбери скин:");
        if (ImGui::BeginCombo("##skin", weapons[selectedWeapon].skins[selectedSkin].name.c_str()))
        {
            for (int n = 0; n < 2; n++)
            {
                bool is_selected = (selectedSkin == n);
                if (ImGui::Selectable(weapons[selectedWeapon].skins[n].name.c_str(), is_selected))
                    selectedSkin = n;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Checkbox("Show overlay in OBS", &showOverlayInOBS);

        ImGui::Text("Текущий выбор: %s - %s", weapons[selectedWeapon].name.c_str(),
                    weapons[selectedWeapon].skins[selectedSkin].name.c_str());

        ImGui::End();

        ImGui::Render();
        context->OMSetRenderTargets(1, &renderTargetView, NULL);
        context->ClearRenderTargetView(renderTargetView, (float[]){0.1f, 0.1f, 0.1f, 1.0f});
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        swapChain->Present(1, 0);
    }

    // Очистка
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    renderTargetView->Release();
    swapChain->Release();
    context->Release();
    device->Release();

    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}
