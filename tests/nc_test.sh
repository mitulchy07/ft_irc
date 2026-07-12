#!/bin/sh

HOST=${1:-127.0.0.1}
PORT=${2:-6667}

exec nc -C "$HOST" "$PORT"
