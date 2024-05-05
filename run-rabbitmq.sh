#!/bin/sh

set -eu

readonly curdir="$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)"
readonly rabbitmq_version='3.13.2'
readonly rabbitmq_etc="$curdir/rabbitmq_server-$rabbitmq_version/etc/rabbitmq"
readonly rabbitmq_sbin="$curdir/rabbitmq_server-$rabbitmq_version/sbin"

sed "s|@@PWD@@|$PWD|" "$curdir/rabbitmq.conf.in" > "$rabbitmq_etc/rabbitmq.conf"

if [ ! -d "$curdir/rabbitmq_server-$rabbitmq_version" ]
then
    curl -LO "https://github.com/rabbitmq/rabbitmq-server/releases/download/v$rabbitmq_version/rabbitmq-server-generic-unix-$rabbitmq_version.tar.xz"
    tar xf "rabbitmq-server-generic-unix-$rabbitmq_version.tar.xz"
    "$rabbitmq_sbin/rabbitmq-plugins" enable rabbitmq_management
fi

"$rabbitmq_sbin/rabbitmq-server"
