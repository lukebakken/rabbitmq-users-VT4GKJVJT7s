$ProgressPreference = 'Continue'
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version 2.0

Write-Host "[INFO] script directory: $PSScriptRoot"

$tls_server_src = Join-Path -Path $PSScriptRoot -ChildPath 'src' | Join-Path -ChildPath 'tls_server.erl'

& erlc +debug $tls_server_src

& erl -noinput -kernel logger_level debug -s tls_server start
