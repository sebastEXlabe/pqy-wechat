$xwechatPath = "$env:APPDATA\Tencent\xwechat"
Write-Host "=== xwechat dir ==="
if (Test-Path $xwechatPath) {
    Get-ChildItem $xwechatPath -Directory | ForEach-Object { Write-Host "DIR: $($_.Name)" }
    Write-Host "--- xlog files ---"
    Get-ChildItem $xwechatPath -Recurse -ErrorAction SilentlyContinue | Where-Object { $_.Name -like "*.xlog" -or $_.Name -like "*.mmkv" } | ForEach-Object { Write-Host "$([math]::Round($_.Length/1KB))KB $($_.FullName)" }
}

$logPath = "$env:APPDATA\Tencent\Logs"
Write-Host "=== Logs dir ==="
if (Test-Path $logPath) {
    Get-ChildItem $logPath -Directory | ForEach-Object { Write-Host "DIR: $($_.Name)" }
    Get-ChildItem $logPath -File -Recurse -ErrorAction SilentlyContinue | ForEach-Object { Write-Host "$([math]::Round($_.Length/1KB))KB $($_.FullName)" }
}

$wechatPath = "$env:APPDATA\Tencent\WeChat"
Write-Host "=== WeChat dir ==="
if (Test-Path $wechatPath) {
    Get-ChildItem $wechatPath -Directory -ErrorAction SilentlyContinue | ForEach-Object { Write-Host "DIR: $($_.Name)" }
}

$qimeiPath = "$env:APPDATA\Tencent\qimei"
Write-Host "=== qimei dir ==="
if (Test-Path $qimeiPath) {
    Get-ChildItem $qimeiPath -File -ErrorAction SilentlyContinue | ForEach-Object { Write-Host "$($_.Name): $([math]::Round($_.Length/1KB))KB" }
}
