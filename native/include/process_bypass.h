#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <set>
#include <string>

namespace pqy {

// 进程枚举相关函数类型
using CreateToolhelp32Snapshot_t = HANDLE(WINAPI*)(DWORD, DWORD);
using Process32FirstW_t = BOOL(WINAPI*)(HANDLE, LPPROCESSENTRY32W);
using Process32NextW_t = BOOL(WINAPI*)(HANDLE, LPPROCESSENTRY32W);
using Module32FirstW_t = BOOL(WINAPI*)(HANDLE, LPMODULEENTRY32W);
using Module32NextW_t = BOOL(WINAPI*)(HANDLE, LPMODULEENTRY32W);

// Hook 函数
HANDLE WINAPI Hook_CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID);
BOOL WINAPI Hook_Process32FirstW(HANDLE hSnapshot, LPPROCESSENTRY32W lppe);
BOOL WINAPI Hook_Process32NextW(HANDLE hSnapshot, LPPROCESSENTRY32W lppe);
BOOL WINAPI Hook_Module32FirstW(HANDLE hSnapshot, LPMODULEENTRY32W lpme);
BOOL WINAPI Hook_Module32NextW(HANDLE hSnapshot, LPMODULEENTRY32W lpme);

// 安装/卸载 Hook
bool InstallProcessBypass();
bool UninstallProcessBypass();

// 配置
void AddHiddenProcess(const std::string& name);
void AddHiddenModule(const std::string& name);
void ClearHiddenLists();

} // namespace pqy
