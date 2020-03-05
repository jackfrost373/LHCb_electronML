
import ROOT
from array import array
from math import sqrt

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetPalette(55)

#f = ROOT.TFile.Open("output/DaVinci_PgunElectrons.root")
#tree = f.Get("electrons/DecayTree")
f = ROOT.TFile.Open("output/DaVinci_Bd2Kstee_electrons.root")
tree = f.Get("Bd_Kstee/DecayTree")

bins = 50

zoom = 0 # zoom around electron track @ ECAL. set to 0 for full ECAL view


###################################

cECAL = ROOT.TCanvas("cECAL")

for entry in tree :

  #clusterxlist = entry.ECAL_cluster_x_arr
  #clusterylist = entry.ECAL_cluster_y_arr
  #clusterelist = entry.ECAL_cluster_e_arr
  photonxlist = entry.ECAL_photon_x_arr
  photonylist = entry.ECAL_photon_y_arr
  photonptlist = entry.ECAL_photon_PT_arr
  photoncllist = entry.ECAL_photon_CL_arr

  xwindow = [-4000, 4000]
  ywindow = [-3200, 3200]
  if(zoom) :
    meanx = entry.eminus_ECAL_x
    meany = entry.eminus_ECAL_y
    xwindow = [meanx - zoom, meanx + zoom]
    ywindow = [meany - zoom, meany + zoom]
 
  # fill clusters
  #hECAL = ROOT.TH2F("hECAL","ECAL clusters", bins,-4000,4000, bins,-3200,3200)
  #for i in range(entry.N_ECAL_clusters) :
  #  hECAL.Fill( clusterxlist[i], clusterylist[i], clusterelist[i] )

  # fill photons
  hECAL = ROOT.TH2F("hECAL","ECAL photons", bins, xwindow[0], xwindow[1], bins, ywindow[0], ywindow[1])
  for i in range(entry.N_ECAL_photons) :
    #if(entry.ECAL_photon_CL_arr[i] > 0.1 and entry.ECAL_photon_PT_arr[i] > 75.) : # BremAdder def. cuts
      hECAL.Fill( photonxlist[i], photonylist[i], photonptlist[i] )
      #hECAL.Fill( photonxlist[i], photonylist[i], photoncllist[i] )

  hECAL.GetXaxis().SetTitle("x [mm]")
  hECAL.GetYaxis().SetTitle("y [mm]")
  #hECAL.GetZaxis().SetTitle("E")
  hECAL.GetZaxis().SetTitle("PT")
  #hECAL.GetZaxis().SetTitle("CL")
  hECAL.SetMinimum(0.)
  #hECAL.SetMaximum(20000.)
  hECAL.SetMaximum(1000.)
  #hECAL.SetMaximum(1.)
  hECAL.Draw("COLZ")
  cECAL.SetFixedAspectRatio()

  electron_x = entry.eminus_ECAL_x
  electron_y = entry.eminus_ECAL_y
  electrongraph = ROOT.TGraph(1, array('f',[electron_x]), array('f',[electron_y]))
  electrongraph.SetMarkerStyle(43)
  electrongraph.SetMarkerSize(3)
  electrongraph.Draw("Psame")
  
  electron_velo_x = entry.eminus_ECAL_velotrack_x 
  electron_velo_y = entry.eminus_ECAL_velotrack_y 
  electron_TT_x = entry.eminus_ECAL_TTtrack_x 
  electron_TT_y = entry.eminus_ECAL_TTtrack_y 
  if( electron_velo_x <= electron_TT_x ) : xslope = 1.
  if( electron_velo_x > electron_TT_x ) : xslope = -1.
  if( electron_velo_y <= electron_TT_y ) : yslope = 1.
  if( electron_velo_y > electron_TT_y ) : yslope = -1.
  electron_velo_x -= 2*xslope*entry.eminus_ECAL_velotrack_sprx
  electron_velo_y -= 2*yslope*entry.eminus_ECAL_velotrack_spry
  electron_TT_x += 2*xslope*entry.eminus_ECAL_TTtrack_sprx
  electron_TT_y += 2*yslope*entry.eminus_ECAL_TTtrack_spry
  electronvelograph = ROOT.TGraph(1, array('f',[electron_velo_x]), array('f',[electron_velo_y]))
  electronvelograph.SetMarkerStyle(27)
  electronvelograph.SetMarkerColor(8)
  electronvelograph.SetMarkerSize(1)
  electronvelograph.Draw("Psame")
  electronTTgraph = ROOT.TGraph(1, array('f',[electron_TT_x]), array('f',[electron_TT_y]))
  electronTTgraph.SetMarkerStyle(27)
  electronTTgraph.SetMarkerColor(9)
  electronTTgraph.SetMarkerSize(1)
  electronTTgraph.Draw("Psame")
 
  if entry.eminus_MCphotondaughters_N > 0 :
    mcphotonxlist = entry.eminus_MCphotondaughters_ECAL_X
    mcphotonylist = entry.eminus_MCphotondaughters_ECAL_Y
    #mcphotonelist = entry.eminus_MCphotondaughters_P
    mcphotongraph = ROOT.TGraph(entry.eminus_MCphotondaughters_N, array('f',mcphotonxlist), array('f',mcphotonylist))
    mcphotongraph.SetMarkerStyle(4)
    mcphotongraph.SetMarkerSize(2)
    mcphotongraph.Draw("Psame")
  
  cECAL.Update()
 

  print("-----------------------------------------------------------------")

  eminus_TRUEP = sqrt(entry.eminus_TRUEPT**2 + entry.eminus_TRUEP_Z**2)
  print("Electron momenta: {0:.0f} (MCtruth), {1:.0f} reco (nobrem), {2:.0f} reco (w/brem)".format(eminus_TRUEP, entry.eminus_nobrem_P, entry.eminus_P))

  print(" * HasBremAdded: {0} (E = {1:.0f}, origin = ({2:.2f},{3:.2f},{4:.2f}))".format(
    entry.eminus_BremMultiplicity, entry.eminus_BremPE, 
    entry.eminus_BremOriginX, entry.eminus_BremOriginY, entry.eminus_BremOriginZ ))

  if(entry.eminus_MCphotondaughters_N > 0) : 
    print(" * Has MC photon daughters: {0}".format(entry.eminus_MCphotondaughters_N))
    for i in range(entry.eminus_MCphotondaughters_N) :
      print("  - PT = {0:.0f},  origin = ({1:.2f},{2:.2f},{3:.2f}), atECAL = ({4:.2f},{5:.2f})".format(
        entry.eminus_MCphotondaughters_PT[i], 
        entry.eminus_MCphotondaughters_orivx_X[i], entry.eminus_MCphotondaughters_orivx_Y[i], entry.eminus_MCphotondaughters_orivx_Z[i], 
        entry.eminus_MCphotondaughters_ECAL_X[i], entry.eminus_MCphotondaughters_ECAL_Y[i] ))


  
  raw_input("Press enter for next event")

  hECAL.Delete()

f.Close()

