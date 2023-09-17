// Ref:
// https://stackoverflow.com/questions/59732181/programmatically-set-graphics-performance-for-an-app
// https://learn.microsoft.com/zh-cn/windows/win32/api/dxgi1_6/ne-dxgi1_6-dxgi_gpu_preference

#include <iostream>
#include <format>

#include <dxgi1_6.h>

#include "CLI11.hpp"
#include "RegHelper.h"

int main(int argc, char** argv) {
    CLI::App app;

    app.allow_windows_style_options();

    std::string exePath;
    DXGI_GPU_PREFERENCE GPUPreference = DXGI_GPU_PREFERENCE_UNSPECIFIED;

    bool bUseGlobalSettings = false;

    try {
        app.add_option("-f, --fileName", exePath)
            ->required()
            ->ignore_case();
        app.add_option("-p, --preference", GPUPreference)
            ->required()
            ->ignore_case();

        app.add_flag("-g, --useGlobalSettings", bUseGlobalSettings)
            ->ignore_case();
    }
    catch (const CLI::ConstructionError& e) {
        std::cout << e.get_name() << std::endl;

        return -1;
    }

    try {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError& e) {
        std::cout << e.get_name() << std::endl;

        return -1;
    }

    std::string settings;

    if(bUseGlobalSettings) {
        if (!ReadRegString(HKEY_CURRENT_USER,
            R"(SOFTWARE\Microsoft\DirectX\UserGpuPreferences)",
            "DirectXUserGlobalSettings",
            settings)) {
            std::cout << "Error to read register" << std::endl;

            return -1;
        }
    }

    settings += std::format("GpuPreference={};",
        static_cast<int>(GPUPreference));

    if (!WriteRegString(HKEY_CURRENT_USER,
        R"(SOFTWARE\Microsoft\DirectX\UserGpuPreferences)",
        exePath.c_str(),
        settings)) {
        std::cout << "Error to write register" << std::endl;

        return -1;
    }

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    CreateProcessA(
        exePath.c_str(),
        nullptr,
        NULL,
        NULL,
        false,
        DETACHED_PROCESS,
        NULL,
        NULL,
        &si,
        &pi
    );

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
