#!/bin/sh

for i in `seq 1 750`; do
	str=$(printf 'run_colmap_dense.sh %08d' $i)
	echo $str
	$str
done
