param(
    [string]$VcpkgRoot = 'C:\vcpkg',
    [string]$Triplet = 'x64-windows'
)

$ErrorActionPreference = 'Stop'

function Write-Step {
    param([string]$Message)
    Write-Host
    Write-Host "== $Message" -ForegroundColor Cyan
}

function Require-Command {
    param(
        [string]$Name,
        [string]$InstallHint
    )

    if (-not (Get-Command $Name -ErrorAction SilentlyContinue)) {
        Write-Host "Missing command: $Name" -ForegroundColor Red
        Write-Host $InstallHint
        exit 1
    }
}

function Assert-NativeCommandSucceeded {
    param([string]$Action)

    if ($LASTEXITCODE -ne 0) {
        throw "$Action failed with exit code $LASTEXITCODE."
    }
}

Write-Step 'Checking Visual Studio C++ compiler'
if (-not (Get-Command cl -ErrorAction SilentlyContinue)) {
    Write-Host 'The C++ compiler was not found in this terminal.' -ForegroundColor Red
    Write-Host "Open 'Developer PowerShell for VS 2022' and run this script again."
    exit 1
}
Write-Host 'Visual Studio C++ compiler found.'

Write-Step 'Checking Git'
Require-Command 'git' 'Install Git for Windows, or install the Git component from Visual Studio Installer.'
Write-Host 'Git found.'

$VcpkgExe = Join-Path $VcpkgRoot 'vcpkg.exe'

Write-Step 'Checking vcpkg'
if (Test-Path -LiteralPath $VcpkgExe) {
    Write-Host "vcpkg found: $VcpkgExe"
}
else {
    Write-Host "vcpkg was not found at $VcpkgRoot"
    Write-Host 'Installing vcpkg...'

    $Parent = Split-Path $VcpkgRoot -Parent
    if (-not (Test-Path -LiteralPath $Parent)) {
        New-Item -ItemType Directory -Force -Path $Parent | Out-Null
    }

    git clone https://github.com/microsoft/vcpkg.git $VcpkgRoot
    Assert-NativeCommandSucceeded 'Cloning vcpkg'

    Push-Location $VcpkgRoot
    try {
        & '.\bootstrap-vcpkg.bat'
        Assert-NativeCommandSucceeded 'Bootstrapping vcpkg'
    }
    finally {
        Pop-Location
    }
}

Write-Step 'Checking vcpkg version'
& $VcpkgExe version
Assert-NativeCommandSucceeded 'Checking the vcpkg version'

Write-Step 'Integrating vcpkg with Visual Studio'
& $VcpkgExe integrate install
Assert-NativeCommandSucceeded 'Integrating vcpkg with Visual Studio'

Write-Step 'Checking Boost.Asio package'
$InstalledPackages = & $VcpkgExe list
Assert-NativeCommandSucceeded 'Listing installed vcpkg packages'
$BoostAsioPackage = "boost-asio:$Triplet"

if ($InstalledPackages -match [regex]::Escape($BoostAsioPackage)) {
    Write-Host "$BoostAsioPackage is already installed."
}
else {
    Write-Host "$BoostAsioPackage is not installed."
    Write-Host 'Installing Boost.Asio...'
    & $VcpkgExe install $BoostAsioPackage
    Assert-NativeCommandSucceeded 'Installing Boost.Asio'
}

Write-Step 'Done'
Write-Host 'Boost.Asio setup is complete.' -ForegroundColor Green
Write-Host
Write-Host 'Next test:'
Write-Host '1. Open Visual Studio 2022.'
Write-Host '2. Create or open the C++ project.'
Write-Host '3. Build with the x64 configuration.'
