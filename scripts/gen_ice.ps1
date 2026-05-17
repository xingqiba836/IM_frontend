Add-Type -AssemblyName System.Drawing
$dir = Join-Path $PSScriptRoot "..\res"
New-Item -ItemType Directory -Force -Path $dir | Out-Null
$bmp = New-Object System.Drawing.Bitmap 120, 120
$g = [System.Drawing.Graphics]::FromImage($bmp)
$g.SmoothingMode = 'AntiAlias'
$g.Clear([System.Drawing.Color]::FromArgb(255, 232, 245, 233))
$brush = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 129, 199, 132))
$g.FillEllipse($brush, 20, 25, 80, 70)
$white = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(200, 255, 255, 255))
$g.FillEllipse($white, 35, 42, 20, 18)
$g.FillEllipse($white, 65, 42, 20, 18)
$pen = New-Object System.Drawing.Pen ([System.Drawing.Color]::FromArgb(255, 46, 125, 50), 3)
$g.DrawArc($pen, 42, 58, 36, 22, 0, 180)
$out = Join-Path $dir "ice.png"
$bmp.Save($out, [System.Drawing.Imaging.ImageFormat]::Png)
$g.Dispose()
$bmp.Dispose()
Write-Host "Created $out"
