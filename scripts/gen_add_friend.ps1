Add-Type -AssemblyName System.Drawing
$dir = Join-Path $PSScriptRoot "..\res"
New-Item -ItemType Directory -Force -Path $dir | Out-Null

function Save-Icon($path, $bg, $fg) {
    $bmp = New-Object System.Drawing.Bitmap 28, 28
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.SmoothingMode = 'AntiAlias'
    $g.Clear([System.Drawing.Color]::FromArgb(255, $bg.R, $bg.G, $bg.B))
    $pen = New-Object System.Drawing.Pen ([System.Drawing.Color]::FromArgb(255, $fg.R, $fg.G, $fg.B), 2)
    $g.DrawLine($pen, 14, 8, 14, 20)
    $g.DrawLine($pen, 8, 14, 20, 14)
    $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose()
    $bmp.Dispose()
}

Save-Icon (Join-Path $dir "add_friend_normal.png") ([System.Drawing.Color]::FromArgb(245,245,245)) ([System.Drawing.Color]::FromArgb(120,120,120))
Save-Icon (Join-Path $dir "add_friend_hover.png") ([System.Drawing.Color]::FromArgb(230,230,230)) ([System.Drawing.Color]::FromArgb(7,193,96))
Write-Host "add_friend icons created"
