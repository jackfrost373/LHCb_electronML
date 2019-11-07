
fname = "PgunElectrons"

from Gaudi.Configuration import *
from Configurables import Boole, LHCbApp

# L0 TCK for MC11
#from Configurables import L0Conf
#L0Conf().TCK = '0x0037'

#-- File catalogs
FileCatalog().Catalogs = [ "xmlcatalog_file:output/NewCatalog.xml" ]

# Use same tags as Gauss (from Gauss/options/Gauss-2012.py) 
LHCbApp().DDDBtag   = "dddb-20180726-2"
LHCbApp().CondDBtag = "sim-20160321-2-vc-md100"
LHCbApp().Simulation = True

# output

HistogramPersistencySvc().OutputFile = "output/Boole_{0}-histos.root".format(fname)
OutputStream("DigiWriter").Output = "DATAFILE='PFN:output/Boole_{0}.digi' TYP='POOL_ROOTTREE' OPT='REC'".format(fname)

# Input
from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles(['PFN:output/Gauss_{0}.sim'.format(fname)], clear=True)

# Just instantiate the configurable...
theApp = Boole()

