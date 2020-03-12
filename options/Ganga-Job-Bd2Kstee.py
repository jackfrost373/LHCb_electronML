


dataloc = "/MC/2017/Beam6500GeV-2017-MagUp-Nu1.6-25ns-Pythia8/Sim09g/Trig0x62661709/Reco17/Turbo04a-WithTurcal/Stripping29r2NoPrescalingFlagged/11124002/ALLSTREAMS.DST"

jobname = "MC_Bd2Kstee_MagUp_Pythia8_2017"
j = Job(name=jobname)
j.comment = "MagUp_Pythia8_2017"
#j.inputfiles = [ LocalFile('./options/mcdatabase.py') ]

# Set up the required application to run
app = "DaVinci"
version  = "v45r2" #"v44r5"
platform = "x86_64-centos7-gcc8-opt" #"x86_64-slc6-gcc62-opt"
projectpath = "/project/bfys/jdevries/cmtuser/LHCb_electronML"

from os import path
if not path.isdir("{0}/{1}Dev_{2}".format(projectpath,app,version)) :
  # warning: below does not compile non-default versions of platform. Manually do LbLogin and make.
  prepareGaudiExec(app, version, myPath=projectpath)
j.application = GaudiExec()
j.application.platform = platform
j.application.directory = "{0}/{1}Dev_{2}".format(projectpath,app,version) 
j.application.options = ['./options/DaVinci-Job-Bd2Kstee.py']
  
#j.backend = Local() # for local testing
j.backend = Dirac() # run on grid

j.outputfiles = [LocalFile('*.root'), LocalFile('stdout')]
#j.outputfiles = [DiracFile('*.root')] # stores on SE. Can download to local with j.outputfiles.get().

j.splitter = SplitByFiles(filesPerJob=20) #11124002 2017: 1646 subjobs, 4M events.
#j.do_auto_resubmit = True  # resubmit failed subjobs. Only turn on if you're very sure.

# set up an auto-merger for the output files
j.postprocessors.append(RootMerger(files = ['*.root'], ignorefailed = True, overwrite = True))

# Get data to run over
print("Querying for data {0}".format(dataloc))
query = BKQuery(dataloc)

if not query: 
  print("Query resulted in nonetype, please check if location is correct.")
  #j.remove()
else :
  j.inputdata = query.getDataset()
  #j.inputdata = query.getDataset()[:3] # for local testing

  j.submit()
  #queues.add(j.submit)

