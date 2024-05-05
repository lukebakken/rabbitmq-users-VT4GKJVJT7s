#!/bin/sh

set -eu

curdir="$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)"
readonly curdir
readonly rabbitmq_version='3.13.2'
readonly rabbitmq_sbin="$curdir/rabbitmq_server-$rabbitmq_version/sbin"

"$rabbitmq_sbin/rabbitmqctl" add_user 'shostakovich-internal.bakken.io' 'dummy'
"$rabbitmq_sbin/rabbitmqctl" set_permissions 'shostakovich-internal.bakken.io' '.*' '.*' '.*'
