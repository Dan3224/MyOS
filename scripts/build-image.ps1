param(
    [string]$Distro = "Ubuntu-20.04"
)

$ErrorActionPreference = "Stop"

$projectPath = Split-Path -Parent $PSScriptRoot
$driveLetter = (Split-Path -Qualifier $projectPath).TrimEnd(":").ToLower()
$relativePath = $projectPath.Substring(2).Replace("\", "/")
$wslProject = "/mnt/$driveLetter$relativePath"
$linuxImage = "/tmp/myos-1.0.iso"
$windowsImage = Join-Path $projectPath "build\myos-1.0.iso"
$transferImage = Join-Path ([System.IO.Path]::GetTempPath()) ("myos-transfer-" + [guid]::NewGuid() + ".iso")

try {
    Write-Host "Building MyOS in WSL..."
    & wsl -d $Distro -- bash -lc "cd '$wslProject' && make clean && make check && cp build/out/myos.elf build/iso/boot/myos.elf && rm -f '$linuxImage' && grub-mkrescue -o '$linuxImage' build/iso"
    if ($LASTEXITCODE -ne 0) {
        throw "The MyOS build failed."
    }

    Copy-Item -LiteralPath "\\wsl$\$Distro\tmp\myos-1.0.iso" -Destination $transferImage
    & wsl --shutdown
    Copy-Item -LiteralPath $transferImage -Destination $windowsImage -Force
    Write-Host "Ready: $windowsImage"
}
finally {
    if (Test-Path -LiteralPath $transferImage) {
        Remove-Item -LiteralPath $transferImage -Force
    }
}
