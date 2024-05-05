#!/bin/sh

set -eu

readonly curdir="$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)"
readonly rabbitmq_version='3.13.2'
readonly rabbitmq_etc="$curdir/rabbitmq_server-$rabbitmq_version/etc/rabbitmq"
readonly rabbitmq_sbin="$curdir/rabbitmq_server-$rabbitmq_version/sbin"

if [ ! -d "$curdir/rabbitmq_server-$rabbitmq_version" ]
then
    curl -LO "https://github.com/rabbitmq/rabbitmq-server/releases/download/v$rabbitmq_version/rabbitmq-server-generic-unix-$rabbitmq_version.tar.xz"
    tar xf "rabbitmq-server-generic-unix-$rabbitmq_version.tar.xz"
fi

sed -e "s|@@PWD@@|$curdir|" "$curdir/rabbitmq.conf.in" > "$rabbitmq_etc/rabbitmq.conf"
cp -vf "$curdir/rmq/enabled_plugins" "$rabbitmq_etc/enabled_plugins"

"$rabbitmq_sbin/rabbitmq-server"
