#!/usr/bin/bash

rm -rf ./build

~/bin/uncrustify -c ./uncrustify.cfg ./*.c --replace --no-backup

