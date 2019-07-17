#!/bin/sh

for i in `seq 1 750`; do
	#rm -r $(printf '%08d/mvs' $i)
	
	str=$(printf 'run_colmap_dense1.sh %08d' $i)
	echo $str
	$str
done
