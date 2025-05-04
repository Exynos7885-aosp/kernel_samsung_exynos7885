#!/bin/bash

set -ex

OUTDIR=out

for def in arch/arm64/configs/*_defconfig; do
	rm -rf $OUTDIR
	make O=$OUTDIR $(basename $def) $@
	cp $OUTDIR/.config $OUTDIR/$(basename $def);
	make O=$OUTDIR savedefconfig;
	cp $OUTDIR/defconfig $def;
done
