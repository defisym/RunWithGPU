#pragma once

#include <string>
#include <Windows.h>

inline bool ReadRegString(HKEY hkey,
    LPCSTR lpSubKey,
    LPCSTR lpValue,
    std::string& retStr) {
    do {
        DWORD dwBufSz = 0;
        auto ret = RegGetValueA(hkey,
            lpSubKey,
            lpValue,
            RRF_RT_REG_SZ,
            nullptr,
            nullptr,
            &dwBufSz
        );

        if (ERROR_SUCCESS != ret) { break; }

		const auto pValue = new char[dwBufSz];
        memset(pValue, '\0', dwBufSz);

        ret = RegGetValueA(hkey,
            lpSubKey,
            lpValue,
            RRF_RT_REG_SZ,
            nullptr,
            (void*)pValue,
            &dwBufSz
        );

        if (ERROR_SUCCESS != ret) { break; }

        retStr = pValue;
        delete[] pValue;

        return true;
    } while (false);

    std::cout << "Error to read register" << std::endl;

    return false;
}

inline bool WriteRegString(HKEY hkey,
    LPCSTR lpSubKey,
    LPCSTR lpValue,
    const std::string& retStr) {
    do {
        HKEY hK;
        auto ret = RegCreateKeyA(hkey,
            lpSubKey,
            &hK
        );
        if (ERROR_SUCCESS != ret) { break; }

        ret = RegSetValueExA(hK,
            lpValue,
            NULL,
            REG_SZ,
            (BYTE*)retStr.c_str(),
            retStr.length());

        if (ERROR_SUCCESS != ret) { break; }

        RegCloseKey(hK);

        return true;
    } while (false);

    std::cout << "Error to write register" << std::endl;

    return false;
}