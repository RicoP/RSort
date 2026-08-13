#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")"

gcc -Wall -O2 -o a.out main.c
./a.out
