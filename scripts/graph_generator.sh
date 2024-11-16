#!/bin/bash
i=1;
for graph in "$@" 
do
	echo "graph - $i: $graph";
	gvpack -u $graph | dot -Tpng -o ${graph}_image.png
	convert ${graph}_image.png -channel RGB -negate ${graph}_neg_image.png
	rm -rf ${graph}_image.png
	rm -rf ${graph}
	i=$((i + 1));
done
