# Phase 2 只读检查脚本 — 零风险
Write-Host "=== Weixin Registry ==="
Get-ItemProperty -Path "HKCU:\Software\Tencent\Weixin" -ErrorAction SilentlyContinue | Format-List

Write-Host "=== Weixin Processes ==="
Get-Process -Name "Weixin" -ErrorAction SilentlyContinue | Select-Object Id,ProcessName,@{N='Threads';E={$_.Threads.Count}},@{N='Mem_MB';E={[math]::Round($_.WorkingSet64/1MB,2)}},StartTime | Format-Table

Write-Host "=== Weixin Modules (key DLLs) ==="
$proc = Get-Process -Name "Weixin" -ErrorAction SilentlyContinue | Select-Object -First 1
$proc.Modules | Where-Object { $_.ModuleName -match "Weixin|mmojo|Voip|XPlugin|xweb" } | Select-Object ModuleName,@{N='Size_KB';E={[math]::Round($_.Size/1KB)}} | Format-Table

Write-Host "=== Xlog Files ==="
$logDir = "$env:APPDATA\Tencent\Weixin"
Get-ChildItem -Path $logDir -Filter "*.xlog" -Recurse -ErrorAction SilentlyContinue | Select-Object @{N='Size_KB';E={[math]::Round($_.Length/1KB)}},FullName -First 20 | Format-Table -AutoSize

Write-Host "=== MMKV Files ==="
Get-ChildItem -Path $logDir -Filter "*.mmkv" -Recurse -ErrorAction SilentlyContinue | Select-Object @{N='Size_KB';E={[math]::Round($_.Length/1KB)}},FullName -First 20 | Format-Table -AutoSize

Write-Host "=== Weixin Install Dir ==="
$installDir = (Get-ItemProperty -Path "HKCU:\Software\Tencent\Weixin" -ErrorAction SilentlyContinue).InstallPath
if ($installDir) {
    Get-ChildItem -Path $installDir -Filter "*.dll" -ErrorAction SilentlyContinue | Select-Object Name,@{N='Size_MB';E={[math]::Round($_.Length/1MB,2)}} | Format-Table
}

Write-Host "=== Done ==="
