#!/bin/sh

set -eu

_tmp="$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)"
readonly script_dir="$_tmp"
unset _tmp

# Set up Python virtualenv and install paho.mqtt
readonly venv_dir="$script_dir/venv"
readonly venv_activate="$script_dir/venv/bin/activate"
if [ ! -x "$venv_activate" ]
then
    python -m venv "$venv_dir"
fi

# shellcheck disable=SC1090
. "$venv_activate"
pip install paho.mqtt

# Start Python program
python "$script_dir/mqtt.py"
