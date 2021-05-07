#!/usr/bin/env bash

SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
BUILDDIR=$SCRIPTDIR/cmake-build-fuzz
CORPUSDIR=$SCRIPTDIR/corpus

"$BUILDDIR"/fuzz_message "$CORPUSDIR" > /dev/null