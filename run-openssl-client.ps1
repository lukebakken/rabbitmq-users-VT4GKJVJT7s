param([string]$TlsHost='localhost', [int]$TlsPort=8883)

$ProgressPreference = 'Continue'
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version 2.0

Write-Host "[INFO] Connecting to TLS host: $TlsHost, TLS port: $TlsPort"

$certs_dir = Join-Path -Path $PSScriptRoot -ChildPath 'certs'
$ca_file   = Join-Path -Path $certs_dir -ChildPath 'ca_certificate.pem'
$cert_file = Join-Path -Path $certs_dir -ChildPath 'client_prokofiev.bakken.io_certificate.pem'
$key_file  = Join-Path -Path $certs_dir -ChildPath 'client_prokofiev.bakken.io_key.pem'

# & openssl s_client -connect "${TlsHost}:${TlsPort}" -CAfile $ca_file -verify 8
& openssl s_client -connect "${TlsHost}:${TlsPort}" -CAfile $ca_file -cert $cert_file -key $key_file -verify 8
