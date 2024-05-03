$ProgressPreference = 'Continue'
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version 2.0

New-Variable -Name curdir -Option Constant `
  -Value (Split-Path -Parent $MyInvocation.MyCommand.Definition)

Write-Host "[INFO] script directory: $curdir"

New-Variable -Name rabbitmq_version -Option Constant -Value '3.13.2'

$rabbitmq_dir = Join-Path -Path $curdir -ChildPath "rabbitmq_server-$rabbitmq_version"
$rabbitmq_sbin = Join-Path -Path $rabbitmq_dir -ChildPath 'sbin'
$rabbitmqctl_cmd = Join-Path -Path $rabbitmq_sbin -ChildPath 'rabbitmqctl.bat'

$rabbitmq_base = Join-Path -Path $curdir -ChildPath 'rmq'

try
{
    $env:RABBITMQ_BASE = $rabbitmq_base
    try
    {
        $ErrorActionPreference = 'Continue'
        & $rabbitmqctl_cmd shutdown
    }
    finally
    {
        $ErrorActionPreference = 'Stop'
    }
}
finally
{
    Remove-Item -Path env:\RABBITMQ_BASE
}
