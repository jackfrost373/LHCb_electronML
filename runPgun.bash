#!/bin/bash

### GAUSS 
##./run gaudirun.py $GAUSSOPTS/Gauss-2012.py  $DECFILESROOT/options/51000034.py $LBPGUNSROOT/options/PGuns.py $GAUSSOPTS/Gauss-Job.py
./GaussDev_v53r2/run gaudirun.py options/Gauss-2012.py options/51000034.py options/PGuns.py options/Gauss-Job.py


### BOOLE
./BooleDev_v33r1/run gaudirun.py options/Boole-Job.py


### BRUNEL
./BrunelDev_v55r2/run gaudirun.py options/Brunel-Job.py


### DAVINCI
./DaVinciDev_v45r2/run gaudirun.py options/DaVinci-Job.py


### ROOT analysis
#lb-run ROOT python analysis/plotEcal.py





# dump DST TES locations
#lb-run Bender/latest dst-dump -d 2012 -f -n 100 output/Brunel_PgunElectrons.dst

