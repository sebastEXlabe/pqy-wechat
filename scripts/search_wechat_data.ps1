Write-Host "=== WeChat Files Data ==="
$basePath = "C:\Users\woshi\Documents\WeChat Files\wxid_41sa28s7rbrl22"
if (Test-Path $basePath) {
    $files = Get-ChildItem -Path $basePath -Recurse -ErrorAction SilentlyContinue | Where-Object { $_.Extension -in '.xlog','.mmkv','.log','.ini','.dat' } | Select-Object -First 30
    foreach ($f in $files) {
        $kb = [math]::Round($f.Length/1KB)
        Write-Host "  $kb KB - $($f.FullName)"
    }
}

Write-Host "=== Top-level WeChat Files ==="
Get-ChildItem -Path "C:\Users\woshi\Documents\WeChat Files" -Directory -ErrorAction SilentlyContinue | ForEach-Object { Write-Host $_.Name }

Write-Host "=== All user data dirs ==="
Get-ChildItem -Path "C:\Users\woshi\Documents\WeChat Files\wxid_41sa28s7rbrl22" -Directory -ErrorAction SilentlyContinue | Select-Object -First 20 | ForEach-Object { Write-Host $_.Name }

Write-Host "=== Done ==="
