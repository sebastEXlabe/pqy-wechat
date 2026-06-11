$f = [System.IO.File]::ReadAllBytes((Resolve-Path "data/live/wechat_live.pcapng"))
$s = [System.Text.Encoding]::ASCII.GetString($f)

Write-Host "=== CGI patterns ==="
$patterns1 = @('/cgi-bin/micromsg-bin/newgetdns','/cgi-bin/micromsg-bin/newsync','/cgi-bin/micromsg-bin/newinit','/cgi-bin/micromsg-bin/newauth','/cgi-bin/micromsg-bin/heartbeat','/cgi-bin/micromsg-bin/statreport','/cgi-bin/micromsg-bin/kvreport')
foreach ($p in $patterns1) {
    if ($s -match $p) { Write-Host "FOUND: $p" }
}

Write-Host "=== Domain patterns ==="
$patterns2 = @('long.weixin.qq.com','short.weixin.qq.com','minorshort.weixin.qq.com','extshort.weixin.qq.com','sgshort.wechat.com','sglong.wechat.com','hklong.weixin.qq.com','szlong.weixin.qq.com')
foreach ($p in $patterns2) {
    if ($s -match $p) { Write-Host "FOUND: $p" }
}

Write-Host "=== Client version ==="
if ($s -match 'clientversion=(\d+)') { Write-Host "clientversion=$($Matches[1])" }
if ($s -match 'uin=(\d+)') { Write-Host "uin=$($Matches[1])" }
if ($s -match 'mccmnc=(\d+)') { Write-Host "mccmnc=$($Matches[1])" }
if ($s -match 'devicetype=([^& ]+)') { Write-Host "devicetype=$($Matches[1])" }
if ($s -match 'sigver=(\d+)') { Write-Host "sigver=$($Matches[1])" }

Write-Host "=== TLS Cipher ==="
if ($s -match 'TLS_AES_256_GCM_SHA384') { Write-Host "FOUND TLS_AES_256_GCM_SHA384" }
if ($s -match 'TLS_CHACHA20_POLY1305_SHA256') { Write-Host "FOUND CHACHA20" }
if ($s -match 'TLS_AES_128_GCM_SHA256') { Write-Host "FOUND AES_128_GCM" }

Write-Host "=== Done ==="
