#!/bin/sh

set -eu

readonly curdir="$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)"
readonly tlshost="${1:-*}"
readonly tlsport="${2:-8883}"

readonly certs_dir="$curdir/certs"
readonly ca_file="$certs_dir/ca_certificate.pem"
readonly cert_file="$certs_dir/client_shostakovich-internal.bakken.io_certificate.pem"
readonly key_file="$certs_dir/client_shostakovich-internal.bakken.io_key.pem"

openssl s_client -connect "$tlshost:$tlsport" -CAfile "$ca_file" -cert "$cert_file" -key "$key_file" -verify 8
