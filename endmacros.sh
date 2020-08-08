#!/usr/bin/env bash
set -euo pipefail

sed -rn 's/\#define\s+([A-Z0-9_]+).*/\#undef \1/p'
