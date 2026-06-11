Write-Host "=== Small config files in xwechat ==="
Get-ChildItem "$env:APPDATA\Tencent\xwechat" -Recurse -ErrorAction SilentlyContinue | Where-Object { $_.Length -lt 500000 -and ($_.Extension -in ".ini",".conf",".cfg",".json",".xml",".dat",".txt") } | ForEach-Object { Write-Host "$([math]::Round($_.Length/1KB))KB $($_.FullName)" }

Write-Host "=== Content of smart heartbeat INI ==="
$iniFiles = Get-ChildItem "$env:APPDATA\Tencent\xwechat" -Recurse -ErrorAction SilentlyContinue | Where-Object { $_.Name -like "*heart*" -or $_.Name -like "*smart*" }
foreach ($f in $iniFiles) { Write-Host "--- $($f.FullName) ---" }

Write-Host "=== Config dir ==="
Get-ChildItem "$env:APPDATA\Tencent\xwechat\config" -Recurse -ErrorAction SilentlyContinue | ForEach-Object { Write-Host "$([math]::Round($_.Length/1KB))KB $($_.FullName)" }

Write-Host "=== Net dirs ==="
for ($i=0; $i -le 9; $i++) {
    $netPath = "$env:APPDATA\Tencent\xwechat\net_$i"
    if (Test-Path $netPath) {
        Write-Host "net_${i}:"
        Get-ChildItem $netPath -File -ErrorAction SilentlyContinue | ForEach-Object { Write-Host "  $([math]::Round($_.Length/1KB))KB $($_.Name)" }
    }
}

Write-Host "=== XPlugin config ==="
Get-ChildItem "$env:APPDATA\Tencent\xwechat\XPlugin" -Recurse -ErrorAction SilentlyContinue | Where-Object { $_.Length -lt 100000 } | ForEach-Object { Write-Host "$([math]::Round($_.Length/1KB))KB $($_.FullName)" }
