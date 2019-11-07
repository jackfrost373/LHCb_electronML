
fname = "PgunElectrons"

from Gaudi.Configuration import *
from Configurables import Brunel, LHCbApp, PrChecker, L0Conf

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:output/NewCatalog.xml" ]

# Use same tags as Gauss (from Gauss/options/Gauss-2012.py) 
LHCbApp().DDDBtag   = "dddb-20180726-2"
LHCbApp().CondDBtag = "sim-20160321-2-vc-md100"
LHCbApp().Simulation = True

Brunel().WithMC = True
Brunel().InputType = "DIGI"

# we did not run Moore, so force the TCK check to false. 
L0Conf().EnsureKnownTCK=False

# output
HistogramPersistencySvc().OutputFile = "output/Brunel_{0}-histos.root".format(fname)
OutputStream("DstWriter").Output = "DATAFILE='PFN:output/Brunel_{0}.dst' TYP='POOL_ROOTTREE' OPT='REC'".format(fname)

# input
from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles(['PFN:output/Boole_{0}.digi'.format(fname)], clear=True)

# Just instantiate the configurable...
theApp = Brunel()

#def doIt():
#         GaudiSequencer("CheckPatSeq").Members.append( PrChecker("prChecker") )
#         appendPostConfigAction( doIt )

