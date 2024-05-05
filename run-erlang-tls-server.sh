#!/bin/sh
# shellcheck disable=SC2155
set -eu

curdir="$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)"
readonly curdir
readonly logger_level="${1:-error}"

(cd "$curdir/redbug" && rebar3 clean && rebar3 compile)

erlc +debug "$curdir/src/tls_server.erl"

erl -pa "$curdir/redbug/_build/default/lib/redbug/ebin/" -noinput -kernel logger_level "$logger_level" -tls_server hostname 'shostakovich-internal.bakken.io' -s tls_server start
