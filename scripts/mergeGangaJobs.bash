#!/bin/bash

job=136
subjobs=89 # total subjobs - 1 (starts at 0)

gangadir=/data/bfys/jdevries/gangadir/workspace/jdevries/LocalXML
filename=DaVinci_Bd2Kstee_electrons.root


outfile=${gangadir}/${job}/output/${filename} 
infiles=`seq -s \  -f "${gangadir}/${job}/%g/output/${filename}" 0 $subjobs`

lb-run DaVinci/v50r3 hadd -j -k -f $outfile $infiles


