$pcap = "data/live/wechat_live.pcapng"
$bytes = [System.IO.File]::ReadAllBytes((Resolve-Path $pcap))
$text = [System.Text.Encoding]::ASCII.GetString($bytes)

Write-Host "=== DNS: newgetdns ==="
$matches = [regex]::Matches($text, '/cgi-bin/micromsg-bin/newgetdns[^" "\t\r\n]*')
foreach ($m in $matches) { Write-Host $m.Value }

Write-Host "=== DNS: long.weixin ==="
$matches2 = [regex]::Matches($text, 'long\.weixin\.qq\.com')
foreach ($m in $matches2) { Write-Host $m.Value }

Write-Host "=== DNS: short.weixin ==="
$matches3 = [regex]::Matches($text, 'short\.weixin\.qq\.com')
foreach ($m in $matches3) { Write-Host $m.Value }

Write-Host "=== DNS: mccmnc ==="
$matches4 = [regex]::Matches($text, 'mccmnc=[^&" "\t\r\n]*')
foreach ($m in $matches4) { Write-Host $m.Value }

Write-Host "=== DNS: networkid ==="
$matches5 = [regex]::Matches($text, 'networkid=[^&" "\t\r\n]*')
foreach ($m in $matches5) { Write-Host $m.Value }

Write-Host "=== TLS: SNI ==="
$matches6 = [regex]::Matches($text, 'long\.weixin\.qq\.com|short\.weixin\.qq\.com|minorshort\.weixin\.qq\.com|extshort\.weixin\.qq\.com|sgshort\.wechat\.com|sglong\.wechat\.com')
foreach ($m in $matches6 | Select-Object -Unique) { Write-Host $m.Value }

Write-Host "=== MMTLS cipher ==="
$matches7 = [regex]::Matches($text, 'TLS_AES|CHACHA20|AES_256_GCM')
foreach ($m in $matches7 | Select-Object -Unique) { Write-Host $m.Value }

Write-Host "=== clientversion ==="
$matches8 = [regex]::Matches($text, 'clientversion=[^&" "\t\r\n]*')
foreach ($m in $matches8) { Write-Host $m.Value }

Write-Host "=== uin ==="
$matches9 = [regex]::Matches($text, '[?&]uin=[^&" "\t\r\n]*')
foreach ($m in $matches9 | Select-Object -First 3) { Write-Host $m.Value }
