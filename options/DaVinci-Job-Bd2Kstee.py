
fname = "Bd2Kstee_electrons"

from Configurables import DaVinci, LHCbApp
from Configurables import DecayTreeTuple, TupleToolDecay, L0Conf
from DecayTreeTuple.Configuration import *
from Gaudi.Configuration import *

# Use same tags as production 
LHCbApp().DDDBtag = 'dddb-20170721-3'
LHCbApp().CondDBtag = 'sim-20190430-1-vc-mu100'
LHCbApp().Simulation = True

stream = "AllStreams" # MC: AllStreams
strip  = "Bu2LLK_eeLine2"

tup = DecayTreeTuple( 'Bd_Kstee' )
#tup.Decay = '[[B0]CC -> ^(J/psi(1S) -> ^e+ ^e-) ^(K*(892)0 -> ^K+ ^pi-)]CC'
tup.Decay = '[[B0]CC -> (J/psi(1S) -> e+ ^e-) (K*(892)0 -> K+ pi-)]CC'
tup.Inputs = ['/Event/{0}/Phys/{1}/Particles'.format(stream, strip)]
tup.addBranches({ 'B0'    : '[[B0]CC -> (J/psi(1S) -> e+ e-) (K*(892)0 -> K+ pi-)]CC',
                  #'Kaon'  : '[[B0]CC -> (J/psi(1S) -> e+ e-) (K*(892)0 -> ^K+ pi-)]CC',
                  #'Pion'  : '[[B0]CC -> (J/psi(1S) -> e+ e-) (K*(892)0 -> K+ ^pi-)]CC',
                  #'Kstar' : '[[B0]CC -> (J/psi(1S) -> e+ e-) ^(K*(892)0 -> K+ pi-)]CC',
                  #'Jpsi'  : '[[B0]CC -> ^(J/psi(1S) -> e+ e-) (K*(892)0 -> K+ pi-)]CC',
                  #'eplus' : '[[B0]CC -> (J/psi(1S) -> ^e+ e-) (K*(892)0 -> K+ pi-)]CC',
                  'eminus' : '[[B0]CC -> (J/psi(1S) -> e+ ^e-) (K*(892)0 -> K+ pi-)]CC' 
                  })

tup.ToolList = []
tup.addTupleTool("TupleToolPrimaries") 
tup.addTupleTool("TupleToolEventInfo") 
tup.addTupleTool("TupleToolCaloInfo") # (custom) dump of ECAL clusters
tup.eminus.addTupleTool("TupleToolKinematic")
tup.eminus.addTupleTool("TupleToolPid") 
tup.eminus.addTupleTool("TupleToolGeometry") 
tup.eminus.addTupleTool("TupleToolTrackInfo") 
tup.eminus.addTupleTool("TupleToolBremInfo")   # Brem adder information
tup.eminus.addTupleTool("TupleToolInfoAtCalo") # (custom) info on extrapolated position at ECAL
mctruthtool = tup.eminus.addTupleTool("TupleToolMCTruth")
mctruthtool.ToolList += ["MCTupleToolPhotonDaughters"] # (custom) info of MC bremphoton daughters

nobremtool = tup.eminus.addTupleTool('LoKi::Hybrid::TupleTool/nobremtool') # no-bremadder info
nobremtool.Variables = {
    'nobrem_P'  : 'PPINFO(504,-100,-200)',
    'nobrem_PT' : 'PPINFO(505,-100,-200)',
    }

#calotool = tup.addTupleTool("TupleToolL0Calo/TupleToolL0Calo")
#calotool.WhichCalo = "ECAL"


DaVinci().Simulation = True
DaVinci().DataType = '2017'

DaVinci().Lumi = not DaVinci().Simulation
DaVinci().appendToMainSequence([tup])
DaVinci().PrintFreq = 100
DaVinci().EvtMax = 10000
DaVinci().InputType="DST"

# input
from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles(['PFN:input/00091955_00000001_7.AllStreams.dst'], clear=True)

# output
DaVinci().TupleFile = "output/DaVinci_{0}.root".format(fname)
DaVinci().HistogramFile = 'output/DaVinci_{0}-histos.root'.format(fname)

