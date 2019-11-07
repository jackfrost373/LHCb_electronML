#!/bin/bash

#LbLogin -c x86_64-centos7-gcc8-opt

lb-dev Gauss/v53r2
cd GaussDev_v53r2
git lb-use Gauss
make

cd ..

lb-dev Boole/v33r1
cd BooleDev_v33r1
git lb-use Boole
make

cd ..

lb-dev Brunel/v55r2
cd BrunelDev_v55r2
git lb-use Brunel
make

cd ..

lb-dev DaVinci/v45r2
cd DaVinciDev_v45r2
git lb-use DaVinci
git lb-use Analysis
git lb-use Rec
git lb-checkout Analysis/run2-patches Phys/DecayTreeTupleTracking
cp ../Phys/DecayTreeTupleTracking/src/* ./Phys/DecayTreeTupleTracking/src/
make

cd ..
