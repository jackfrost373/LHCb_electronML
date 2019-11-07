
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

tupletools = []
tupletools.append("TupleToolKinematic")  # Momenta
tupletools.append("TupleToolMCTruth")    # MC Truth info
tupletools.append("TupleToolBremInfo")   # Info on photon that was BremAdded
tupletools.append("TupleToolCaloInfo")   # New tupletool to put CALO info per track
#tupletools.append("TupleToolEventInfo")  # Event info
tup.ToolList =  tupletools[:]
tup.addTool(TupleToolDecay, name="eminus")

nobremtool = tup.eminus.addTupleTool('LoKi::Hybrid::TupleTool/nobremtool')
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

