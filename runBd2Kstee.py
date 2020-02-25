#!/bin/bash

# Run our DaVinci tools over existing Bd2Kstee simulation
# from MC_2017_Beam6500GeV2017MagUpNu1.625nsPythia8_Sim09g_Trig0x62661709_Reco17_Turbo04aWithTurcal_Stripping29r2NoPrescalingFlagged_11124002_ALLSTREAMS.DST
#lhcb-proxy-init
#lb-run LHCbDirac dirac-dms-get-file /lhcb/MC/2017/ALLSTREAMS.DST/00091955/0000/00091955_00000001_7.AllStreams.dst

# Run DaVinci
./DaVinciDev_v45r2/run gaudirun.py options/DaVinci-Job-Bd2Kstee.py

#run analysis
#lb-run ROOT python analysis/plotEcal.py

