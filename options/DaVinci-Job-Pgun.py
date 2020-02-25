
fname = "PgunElectrons"

from Configurables import DaVinci, LHCbApp
from Configurables import DecayTreeTuple, TupleToolDecay, L0Conf
from DecayTreeTuple.Configuration import *
from Gaudi.Configuration import *

# Use same tags as Gauss (from Gauss/options/Gauss-2012.py) 
LHCbApp().DDDBtag   = "dddb-20180726-2"
LHCbApp().CondDBtag = "sim-20160321-2-vc-md100"
LHCbApp().Simulation = True

# we did not run Moore, so force the TCK check to false. 
L0Conf().EnsureKnownTCK=False

tup = DecayTreeTuple( 'electrons' )
tup.Decay = '[e-]CC'
#tup.setDescriptorTemplate('${eminus}[e-]CC')
tup.Inputs = ["Phys/StdAllLooseElectrons/Particles"]
#tup.ReFitPVs = True


tup.ToolList = []
tup.addTupleTool("TupleToolKinematic")  # Momenta
tup.addTupleTool("TupleToolBremInfo")   # Info on photon that was BremAdded
tup.addTupleTool("TupleToolCaloInfo")   # (custom) dump of ECAL clusters
tup.addTupleTool("TupleToolInfoAtCalo") # (custom) info on extrapolated position at ECAL

mctruthtool = tup.addTupleTool("TupleToolMCTruth")
mctruthtool.ToolList += ["MCTupleToolPhotonDaughters"] # (custom) info of MC bremphoton daughters

nobremtool = tup.addTupleTool('LoKi::Hybrid::TupleTool/nobremtool')
nobremtool.Variables = {
    'nobrem_P'  : 'PPINFO(504,-100,-200)',
    'nobrem_PT' : 'PPINFO(505,-100,-200)',
    }
              
#calotool = tup.addTupleTool("TupleToolL0Calo/TupleToolL0Calo")
#calotool.WhichCalo = "ECAL"


DaVinci().Simulation = True
DaVinci().DataType = '2012'

DaVinci().Lumi = not DaVinci().Simulation
DaVinci().appendToMainSequence([tup])
DaVinci().PrintFreq = 100
DaVinci().InputType="DST"

# input
from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles(['PFN:output/Brunel_{0}.dst'.format(fname)], clear=True)

# output
DaVinci().TupleFile = "output/DaVinci_{0}.root".format(fname)
DaVinci().HistogramFile = 'output/DaVinci_{0}-histos.root'.format(fname)

