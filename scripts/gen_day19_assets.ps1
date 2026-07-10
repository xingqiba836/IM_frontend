Add-Type -AssemblyName System.Drawing
$dir = Join-Path $PSScriptRoot "..\res"
New-Item -ItemType Directory -Force -Path $dir | Out-Null

function Save-Png($path, $draw) {
    $bmp = New-Object System.Drawing.Bitmap 32, 32
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.SmoothingMode = 'AntiAlias'
    $draw.Invoke($g)
    $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose()
    $bmp.Dispose()
}

function Save-Jpg($path, $color) {
    $bmp = New-Object System.Drawing.Bitmap 64, 64
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.SmoothingMode = 'AntiAlias'
    $g.Clear($color)
    $brush = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::White)
    $g.FillEllipse($brush, 16, 16, 32, 32)
    $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Jpeg)
    $g.Dispose()
    $bmp.Dispose()
}

Save-Png (Join-Path $dir "search.png") {
    param($g)
    $g.Clear([System.Drawing.Color]::Transparent)
    $pen = New-Object System.Drawing.Pen ([System.Drawing.Color]::FromArgb(150, 150, 150), 2)
    $g.DrawEllipse($pen, 8, 8, 14, 14)
    $g.DrawLine($pen, 20, 20, 26, 26)
}

Save-Png (Join-Path $dir "close_transparent.png") {
    param($g)
    $g.Clear([System.Drawing.Color]::Transparent)
}

Save-Png (Join-Path $dir "close_search.png") {
    param($g)
    $g.Clear([System.Drawing.Color]::Transparent)
    $pen = New-Object System.Drawing.Pen ([System.Drawing.Color]::FromArgb(150, 150, 150), 2)
    $g.DrawLine($pen, 10, 10, 22, 22)
    $g.DrawLine($pen, 22, 10, 10, 22)
}

$colors = @(
    [System.Drawing.Color]::FromArgb(255, 129, 199, 132),
    [System.Drawing.Color]::FromArgb(255, 100, 181, 246),
    [System.Drawing.Color]::FromArgb(255, 255, 183, 77),
    [System.Drawing.Color]::FromArgb(255, 186, 104, 200),
    [System.Drawing.Color]::FromArgb(255, 77, 182, 172)
)
for ($i = 0; $i -lt 5; $i++) {
    Save-Jpg (Join-Path $dir ("head_{0}.jpg" -f ($i + 1))) $colors[$i]
}

Write-Host "day19 assets created"
