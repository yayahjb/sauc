#!/bin/bash
rm -rf data_cifs/$1
mkdir -p data_cifs/$1
rm -rf cod_$1.tsv
touch cod_$1.tsv
for  level2 in cif/$1/*/* 
do
  # echo "level2: " $level2
  ls $level2/*.cif &> /dev/null
  if [ $? == 0 ] 
  then
    for xcif in $level2/*.cif
    do
      xdata=${xcif##*/}
      data_cif=data_cifs/$1/data_${xdata}
      data_zinc=${data_cif}.zinc
      echo "processing $xcif to $data_cif"
      bin/cif2cif -q codfields.req -i $xcif -o $data_cif
      echo "processing $data_cif to $data_zinc"
      Zinc/src/cifZinc -c $data_cif > $data_zinc
      bin/getcodfields < $data_zinc >> cod_$1.tsv
    done
  fi
done

