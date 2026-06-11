Write-Host "=== Searching for xlog files ==="
$paths = @("$env:APPDATA\Tencent\Weixin", "$env:LOCALAPPDATA\Tencent\Weixin", "C:\ProgramData\Tencent\Weixin")
foreach ($p in $paths) {
    Write-Host "Checking: $p"
    if (Test-Path $p) {
        $files = Get-ChildItem -Path $p -Filter "*.xlog" -Recurse -ErrorAction SilentlyContinue
        foreach ($f in $files) {
            $kb = [math]::Round($f.Length/1KB)
            Write-Host "  $kb KB - $($f.FullName)"
        }
    }
}

Write-Host "=== Searching for mmkv files ==="
foreach ($p in $paths) {
    if (Test-Path $p) {
        $files = Get-ChildItem -Path $p -Filter "*.mmkv*" -Recurse -ErrorAction SilentlyContinue
        foreach ($f in $files) {
            $kb = [math]::Round($f.Length/1KB)
            Write-Host "  $kb KB - $($f.FullName)"
        }
    }
}

Write-Host "=== Searching for log/ini files ==="
foreach ($p in $paths) {
    if (Test-Path $p) {
        $files = Get-ChildItem -Path $p -Include "*.log","*.ini","*.txt","*.dat" -Recurse -ErrorAction SilentlyContinue | Select-Object -First 50
        foreach ($f in $files) {
            Write-Host "  $($f.Length) bytes - $($f.FullName)"
        }
    }
}

Write-Host "=== Done ==="
