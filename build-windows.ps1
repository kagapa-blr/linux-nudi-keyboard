[CmdletBinding()]
param(
    [ValidateSet('Release', 'Debug')]
    [string]$Configuration = 'Release',
    [switch]$Clean,
    [switch]$SkipTests,
    [string]$OutputDirectory = 'build-artifacts'
)

$ErrorActionPreference = 'Stop'
$projectDirectory = Split-Path -Parent $MyInvocation.MyCommand.Path
$buildDirectory = Join-Path $projectDirectory 'build-windows'
$outputPath = Join-Path $projectDirectory $OutputDirectory

Set-Location $projectDirectory

function Invoke-Native {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Command,
        [Parameter(Mandatory = $true)]
        [string[]]$Arguments
    )

    & $Command @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "Command failed with exit code $LASTEXITCODE`: $Command $($Arguments -join ' ')"
    }
}

if ($Clean) {
    Remove-Item -LiteralPath $buildDirectory -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item -LiteralPath $outputPath -Recurse -Force -ErrorAction SilentlyContinue
}

New-Item -ItemType Directory -Path $outputPath -Force | Out-Null

Write-Host 'Configuring Nudi for Windows'
Invoke-Native 'cmake' @(
    '-S', $projectDirectory,
    '-B', $buildDirectory,
    '-G', 'Visual Studio 17 2022',
    '-A', 'x64',
    "-DCMAKE_BUILD_TYPE=$Configuration"
)

Write-Host "Building Nudi ($Configuration)"
Invoke-Native 'cmake' @(
    '--build', $buildDirectory,
    '--config', $Configuration,
    '--parallel'
)

if (-not $SkipTests) {
    Write-Host 'Running Windows tests'
    Invoke-Native 'ctest' @(
        '--test-dir', $buildDirectory,
        '--build-config', $Configuration,
        '--output-on-failure',
        '--verbose'
    )
}

$releaseDirectory = Join-Path $buildDirectory $Configuration
$executablePath = Join-Path $releaseDirectory 'nudi-windows.exe'
$testPath = Join-Path $releaseDirectory 'test-composer.exe'

if (-not (Test-Path -LiteralPath $executablePath)) {
    throw "Windows executable was not generated: $executablePath"
}

if (-not $SkipTests -and -not (Test-Path -LiteralPath $testPath)) {
    throw "Windows test executable was not generated: $testPath"
}

$archivePath = Join-Path $outputPath "kannada-nudi-windows-$Configuration.zip"
$filesToPackage = @($executablePath)
if (-not $SkipTests) {
    $filesToPackage += $testPath
}

Compress-Archive -Path $filesToPackage -DestinationPath $archivePath -Force
Write-Host "Windows package written to: $archivePath"
