#include "hook_manager.h"
#include <MinHook.h>
#include <spdlog/spdlog.h>

namespace pqy {

HookManager& HookManager::GetInstance() {
    static HookManager instance;
    return instance;
}

bool HookManager::Init() {
    if (initialized_) {
        return true;
    }

    MH_STATUS status = MH_Initialize();
    if (status != MH_OK) {
        spdlog::error("MH_Initialize failed: {}", MH_StatusToString(status));
        return false;
    }

    initialized_ = true;
    spdlog::info("HookManager initialized");
    return true;
}

void HookManager::Shutdown() {
    if (!initialized_) {
        return;
    }

    DisableAll();
    MH_Uninitialize();
    hooks_.clear();
    initialized_ = false;
    spdlog::info("HookManager shutdown");
}

bool HookManager::InstallHook(HookType type, void* target, void* detour, void** original) {
    if (!initialized_) {
        spdlog::error("HookManager not initialized");
        return false;
    }

    // 检查是否已安装
    if (hooks_.count(static_cast<int>(type)) > 0) {
        spdlog::warn("Hook already installed for type {}", static_cast<int>(type));
        return false;
    }

    // 创建 Hook
    MH_STATUS status = MH_CreateHook(target, detour, original);
    if (status != MH_OK) {
        spdlog::error("MH_CreateHook failed: {}", MH_StatusToString(status));
        return false;
    }

    // 启用 Hook
    status = MH_EnableHook(target);
    if (status != MH_OK) {
        spdlog::error("MH_EnableHook failed: {}", MH_StatusToString(status));
        return false;
    }

    // 记录
    HookInfo info;
    info.type = type;
    info.target = target;
    info.detour = detour;
    info.original = *original;
    info.status = HookStatus::ENABLED;
    hooks_[static_cast<int>(type)] = info;

    spdlog::info("Hook installed: type={}", static_cast<int>(type));
    return true;
}

bool HookManager::UninstallHook(HookType type) {
    auto it = hooks_.find(static_cast<int>(type));
    if (it == hooks_.end()) {
        return false;
    }

    MH_STATUS status = MH_RemoveHook(it->second.target);
    if (status != MH_OK) {
        spdlog::error("MH_RemoveHook failed: {}", MH_StatusToString(status));
        return false;
    }

    hooks_.erase(it);
    spdlog::info("Hook uninstalled: type={}", static_cast<int>(type));
    return true;
}

bool HookManager::EnableHook(HookType type) {
    auto it = hooks_.find(static_cast<int>(type));
    if (it == hooks_.end()) {
        return false;
    }

    MH_STATUS status = MH_EnableHook(it->second.target);
    if (status != MH_OK) {
        return false;
    }

    it->second.status = HookStatus::ENABLED;
    return true;
}

bool HookManager::DisableHook(HookType type) {
    auto it = hooks_.find(static_cast<int>(type));
    if (it == hooks_.end()) {
        return false;
    }

    MH_STATUS status = MH_DisableHook(it->second.target);
    if (status != MH_OK) {
        return false;
    }

    it->second.status = HookStatus::DISABLED;
    return true;
}

bool HookManager::EnableAll() {
    MH_STATUS status = MH_EnableHook(MH_ALL_HOOKS);
    return status == MH_OK;
}

bool HookManager::DisableAll() {
    MH_STATUS status = MH_DisableHook(MH_ALL_HOOKS);
    return status == MH_OK;
}

HookStatus HookManager::GetStatus(HookType type) const {
    auto it = hooks_.find(static_cast<int>(type));
    if (it == hooks_.end()) {
        return HookStatus::DISABLED;
    }
    return it->second.status;
}

std::vector<HookInfo> HookManager::GetAllHooks() const {
    std::vector<HookInfo> result;
    for (const auto& [type, info] : hooks_) {
        result.push_back(info);
    }
    return result;
}

void HookManager::SetRiskCallback(std::function<void(int level)> callback) {
    risk_callback_ = callback;
}

void HookManager::NotifyRiskLevel(int level) {
    if (risk_callback_) {
        risk_callback_(level);
    }
}

} // namespace pqy
