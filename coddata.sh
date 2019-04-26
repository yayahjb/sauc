#!/bin/bash
# creates a new cod.tsv.bz2
for level in `seq 1 9`
do
bash ./exdata.sh $level &
done
wait
cat cod_{1,2,3,4,5,6,7,8,9}.tsv | bzip2 > cod.tsv.bz2

