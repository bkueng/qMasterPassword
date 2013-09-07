#! /bin/sh

cd "$(dirname $(readlink -f $0))"

cp pre-commit ../.git/hooks

