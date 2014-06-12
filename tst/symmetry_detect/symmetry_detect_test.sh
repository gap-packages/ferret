#!/usr/bin/env bash

./jsonToGap.py <(symmetry_detect $1 -q) $2 | gap.sh -q