#!/usr/bin/env bash

bliss $* | $(dirname -- "$0")/fixblissoutput.py
