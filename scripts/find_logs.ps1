Write-Host "=== Install dir xlog/mmkv ==="
Get-ChildItem "C:\Program Files\Tencent\Weixin" -Recurse -ErrorAction SilentlyContinue | Where-Object { $_.Extension -in ".xlog",".mmkv" } | ForEach-Object { Write-Host "$([math]::Round($_.Length/1KB)) KB - $($_.FullName)" }

Write-Host "=== AppData Roaming Tencent ==="
Get-ChildItem "$env:APPDATA\Tencent" -Directory -ErrorAction SilentlyContinue | ForEach-Object { Write-Host $_.Name }

Write-Host "=== AppData Local Tencent ==="
Get-ChildItem "$env:LOCALAPPDATA\Tencent" -Directory -ErrorAction SilentlyContinue | ForEach-Object { Write-Host $_.Name }

Write-Host "=== Documents WeChat Files subdirs ==="
Get-ChildItem "C:\Users\woshi\Documents\WeChat Files\wxid_41sa28s7rbrl22" -Directory -ErrorAction SilentlyContinue | ForEach-Object { Write-Host "  DIR: $($_.Name)" }
Get-ChildItem "C:\Users\woshi\Documents\WeChat Files\wxid_41sa28s7rbrl22" -File -ErrorAction SilentlyContinue | Select-Object -First 20 | ForEach-Object { Write-Host "  FILE $([math]::Round($_.Length/1KB))KB - $($_.Name)" }
