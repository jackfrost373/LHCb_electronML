#!/bin/bash

# Run our DaVinci tools over existing Bd2Kstee simulation
#  from MC_2017_Beam6500GeV2017MagUpNu1.625nsPythia8_Sim09g_Trig0x62661709_Reco17_Turbo04aWithTurcal_Stripping29r2NoPrescalingFlagged_11124002_ALLSTREAMS.DST

# Download file
#lhcb-proxy-init
#lb-run LHCbDirac dirac-dms-get-file /lhcb/MC/2017/ALLSTREAMS.DST/00091955/0000/00091955_00000001_7.AllStreams.dst
#mv *.dst input/

# Run DaVinci locally
#./DaVinciDev_v45r2/run gaudirun.py options/DaVinci-Job-Bd2Kstee.py input/inputLocalFile.py
#mv *.root output/

#run analysis
#lb-run ROOT python analysis/plotEcal.py

# Run DaVinci on grid. Make sure DaVinci().EvtMax is set to -1
ganga options/Ganga-Job-Bd2Kstee.py


