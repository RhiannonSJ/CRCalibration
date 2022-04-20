/************************************************************************
 * 
 * A macro to understand the truth-level hit and track distributions for 
 * dE/dx calibration studies
 *
 *
 * Example file list located here:
 *   /home/jones/work/cosmics/LArSoft-v08_50_00/work/files/anafiles.list
 *
 *
 *************************************************************************/

#include "EventProcessor.h"
#include "ConfigReader.h"

using namespace calib;
using namespace cppsecrets;

// Allowed branches to read from the tree
std::vector<TString> allowed = {
   "run",
   "event",
   "geant_list_size",
   "inTPCActive",
   "TrackId",
   "pdg",
   "Mother",
   "ntracks_pandoraTrack",
   "trkId_pandoraTrack",
   "trkidtruth_pandoraTrack",
   "trkpdgtruth_pandoraTrack",
   "trkg4id_pandoraTrack",
   "ntrkhits_pandoraTrack",
   "trkresrg_pandoraTrack",
   "trkdqdx_pandoraTrack",
   "trkdedx_pandoraTrack",
   "trkke_pandoraTrack",
   "trkxyz_pandoraTrack",
   "trkstartx_pandoraTrack",
   "trkstarty_pandoraTrack",
   "trkstartz_pandoraTrack",
   "trkendx_pandoraTrack",
   "trkendy_pandoraTrack",
   "trkendz_pandoraTrack",
   "trklen_pandoraTrack",
   "no_hits_stored",
   "hit_tpc",
   "hit_plane",
   "hit_charge",
   "hit_energy",
   "hit_nelec",
   "hit_trueX",
   "hit_trkid",
   "hit_peakT",
   "hit_startT",
   "hit_endT",
   "StartPointx",
   "StartPointy",
   "StartPointz",
   "EndPointx",
   "EndPointy",
   "EndPointz",
   "StartPointx_tpcAV",
   "StartPointy_tpcAV",
   "StartPointz_tpcAV",
   "EndPointx_tpcAV",
   "EndPointy_tpcAV",
   "EndPointz_tpcAV",
   "NumberDaughters",
   "P",
   "Eng",
   "EndE",
   "pathlen"
 };

// A translation list from plane labels to longer labels for plotting
std::map<std::string, std::string> planeLabels = {
  {"h0", "APA 1"},
  {"h1", "CPA 1"},
  {"h2", "APA 2"},
  {"h3", "CPA 2"},
  {"h4", "APA 3"},
  {"t",  "Top"},
  {"bo", "Bot."},
  {"f",  "Fro."},
  {"ba", "Back"},
};

typedef std::vector<Plane> PlaneList;
     
int multiProductionAnalysis(const char *config){

  // First, setup timing information so we can monitor the run
  time_t rawtime;
  std::cout << "-----------------------------------------------------------" << std::endl;
  GetTime(rawtime);
  std::cout << "-----------------------------------------------------------" << std::endl;

  //------------------------------------------------------------------------------------------
  //                                    Configure
  //------------------------------------------------------------------------------------------
  // Create object of the class ConfigReader
  // Parse the configuration file
  // Dump map on the console after parsing it
  ConfigReader* p = ConfigReader::getInstance();
  p->parseFile(config);
  std::cout << " Variables from configuration file: " << std::endl;
  p->dumpFileValues();
  std::cout << "-----------------------------------------------------------" << std::endl;

  // Get configuration variables
  int n = -1;
  int verbose = 0; // Whether to print certain functions verbosely or not
  std::string input_list = "";
  std::string location="";
  std::string tag="";
  std::vector<double> minx_fid, miny_fid, minz_fid;
  std::vector<double> maxx_fid, maxy_fid, maxz_fid;
  std::vector<double> minx_av, miny_av, minz_av;
  std::vector<double> maxx_av, maxy_av, maxz_av;

  p->getValue("InputList", input_list);
  p->getValue("Location",  location);
  p->getValue("Tag",       tag);
  p->getValue("NFiles",    n);
  p->getValue("Verbose",   verbose);
  p->getValue("MinXFid",   minx_fid);
  p->getValue("MinYFid",   miny_fid);
  p->getValue("MinZFid",   minz_fid);
  p->getValue("MaxXFid",   maxx_fid);
  p->getValue("MaxYFid",   maxy_fid);
  p->getValue("MaxZFid",   maxz_fid);
  p->getValue("MinXAV",    minx_av);
  p->getValue("MinYAV",    miny_av);
  p->getValue("MinZAV",    minz_av);
  p->getValue("MaxXAV",    maxx_av);
  p->getValue("MaxYAV",    maxy_av);
  p->getValue("MaxZAV",    maxz_av);

  // Get the active and fiducial geometry objects
  Geometry fiducial(minx_fid,miny_fid,minz_fid,maxx_fid,maxy_fid,maxz_fid,true);
  Geometry active(minx_av,miny_av,minz_av,maxx_av,maxy_av,maxz_av,false);
  PlaneList extPlanes = active.GetExternalPlaneList();
  PlaneList allPlanes = active.GetPlaneList();
  PlaneList intPlanes = active.GetInternalPlaneList(allPlanes,extPlanes);
  PlaneList fidExtPlanes = fiducial.GetExternalPlaneList();
  PlaneList fidAllPlanes = fiducial.GetPlaneList();

  std::cout << "-----------------------------------------------------------" << std::endl;
  std::cout << " Total number of planes in the active volume of the DUNE SP module: " << allPlanes.size() << std::endl;
  std::cout << " Consisting of " << extPlanes.size() << " external planes and " << intPlanes.size() << " internal planes" << std::endl; 
  std::cout << "-----------------------------------------------------------" << std::endl;
 
  // Sort out the file tag
  if(tag != "")
    tag = "_"+tag;

  //--------------------------------------------------------------------------------- ---------
  //                                    Initialise
  //--------------------------------------------------------------------------------- ---------

  // Setup vectors for each TTree and set of labels for the productions
  std::vector<TChain*> prodTrees;
  std::vector<anatree*> prodEvents;
  std::vector<std::string> prodFiles, prodLabels, prodTeXLabels;

  ReadCSV(input_list, n, prodFiles, prodLabels, prodTeXLabels, verbose);

  // In the case of the files themselves, pass to the EventProcessor before filling the anatree vector
  // Setup TTrees from input file list
  std::cout << " Reading files and running analysis" << std::endl;

  // Setup maps and vectors of histograms, counters and labels for the overlays and statistics tables
  // The format is: 
  //    Map key: Counter label or histogram label
  //    Map content: Vector of size nProd for each production's entry
  std::map<std::string, std::vector<unsigned int>> prodCounters;
  std::map<std::string, std::vector<TH1D*>> prodHistograms;
  std::vector<std::string> counterLabels{
    "Events",
    "Tracks",
    "Muons",
    "Crosses top or bottom",
    "Crosses top and bottom",
    "Crosses $ \\geq $ 1 APA/CPA",
    "Crosses $ \\geq $ 2 APA/CPA",
    "Stopping",
    "Exiting",
    "$\\mu > 3$~m",
    "Stopping \\& $\\mu > 3$~m",
    "$\\mu > 3$~m \\& $y_{i} > 599.5$~cm"
  };

  const unsigned int nFiles = prodFiles.size();
  std::vector<unsigned int> dummyVec(nFiles,0);
  
  // Sort out the map entries for the TeX file
  prodCounters["Events"] = dummyVec;
  prodCounters["Tracks"] = dummyVec;
  prodCounters["Muons"] = dummyVec;
  prodCounters["Crosses top or bottom"] = dummyVec;
  prodCounters["Crosses top and bottom"] = dummyVec;
  prodCounters["Crosses $ \\geq $ 1 APA/CPA"] = dummyVec;
  prodCounters["Crosses $ \\geq $ 2 APA/CPA"] = dummyVec;
  prodCounters["Stopping"] = dummyVec;
  prodCounters["Exiting"] = dummyVec;
  prodCounters["$\\mu > 3$~m"] = dummyVec;
  prodCounters["Stopping \\& $\\mu > 3$~m"] = dummyVec;
  prodCounters["$\\mu > 3$~m \\& $y_{i} > 599.5$~cm"] = dummyVec;

  unsigned int f   = 0;
  unsigned int iIt = 1;
  unsigned int nEvents = 0;
  for(const std::string &file : prodFiles){
    EventProcessor evtProc(allowed, file, n);
    evtProc.Initialize(false);

    // Now setup the tree and event objects to work with
    TChain *tree = evtProc.GetTree();
    anatree *evt = evtProc.GetEvents();
    prodTrees.push_back(tree);
    prodEvents.push_back(evt);

    // Start of analysis (loop over chain and events
    if(verbose){
      std::cout << " Running analysis over file " << file << std::endl;
      std::cout << " with label: " << prodLabels.at(f) << std::endl;
      std::cout << " and TeX label: " << prodTeXLabels.at(f) << "..." << std::endl << std::endl;
    }

    // Then setup the histograms, counters and any other variables to add to
    // Setup histograms
    TH1D *h_track_length = new TH1D(("h_track_length_"+prodLabels.at(f)).c_str(),"",100,0,2200);

    // Setup counters
    unsigned int totalTracks      = 0;
    unsigned int nMu              = 0;
    unsigned int nLongTracks      = 0;
    unsigned int nLongHighYTracks = 0;
    unsigned int topBottom        = 0;
    unsigned int topOrBottom      = 0;
    unsigned int min2APACPA       = 0;
    unsigned int min1APACPA       = 0;
    unsigned int stopping         = 0;
    unsigned int nStoppingLong    = 0;
    unsigned int exiting          = 0;

    // Now loop over the events
    unsigned int nEvts = tree->GetEntries();

    std::cout << " |";
    for(unsigned int iEvt = 0; iEvt < nEvts; ++iEvt){
      tree->GetEntry(iEvt);
      if(!evtProc.SelectEvent(evt)) continue;
      nEvents++;
      unsigned int nTrks = evt->ntracks_pandoraTrack;
      int nGeant = evt->geant_list_size;

      // Print the processing rate
      double evtFrac  = iEvt*(f+1)/static_cast<double>(nEvts*nFiles);

      // Print status
      if(std::abs(0.1*iIt-evtFrac) < std::numeric_limits<double>::epsilon()){
        std::cout << " --- " << evtFrac*100 << " %";
        std::cout.flush();
        iIt++;
      }// Status

      // Now run the analysis, start by filling a statistics table for the three productions
      for(unsigned int iTrk = 0; iTrk < nTrks; ++iTrk){

        // Setup list of plane labels the track has crossed
        std::vector<std::string> labelsCrossed;

        // Get the best plane
        int bestPlane = 0;
        int currHits  = -999;
        for(int iPlane = 0; iPlane < 3; ++iPlane){
          if(evt->ntrkhits_pandoraTrack[iTrk][iPlane] > currHits){
            currHits  = evt->ntrkhits_pandoraTrack[iTrk][iPlane];
            bestPlane = iPlane; 
          } // CurrHits
        } // Planes

        // Count tracks
        totalTracks++;

        int ID = evt->trkId_pandoraTrack[iTrk];

        // Only look at primary muons
        if(abs(evt->trkpdgtruth_pandoraTrack[iTrk][bestPlane]) != 13) continue;
        nMu++;

        // Get the track geometry
        TVector3 startVtx(evt->trkstartx_pandoraTrack[iTrk],
            evt->trkstarty_pandoraTrack[iTrk],
            evt->trkstartz_pandoraTrack[iTrk]);
        TVector3 endVtx(evt->trkendx_pandoraTrack[iTrk],
            evt->trkendy_pandoraTrack[iTrk],
            evt->trkendz_pandoraTrack[iTrk]);

        // Since we are generating downwards-going tracks - if the start y < end y then 
        // assume the reconstruction has got them the wrong way around and flip them
        if(startVtx.Y() < endVtx.Y()){
          TVector3 temp(endVtx);
          endVtx = startVtx;
          startVtx = temp;
        }

        float length = evt->trklen_pandoraTrack[iTrk];
        h_track_length->Fill(length);

        // Find the closest plane to the start vertex and count it as a crossing plane
        Plane enteringPlane = GetClosestPlane(extPlanes, startVtx, endVtx);
        double distFromEntrance = GetDistanceToPlane(enteringPlane, startVtx, endVtx);

        Plane exitingPlane = GetClosestPlane(extPlanes, endVtx, startVtx);
        double distFromExit = GetDistanceToPlane(exitingPlane, endVtx, startVtx);

        // Now determine if the current track crossed each detector plane individually
        unsigned int planeN    = 0;
        unsigned int extPlaneN = 0;

        // Counter for the number of planes this track has crossed
        unsigned int nPlanesCrossed = 0;
        unsigned int nExtCrossed    = 0;

        // Loop over planes
        for(const Plane &pl : fidAllPlanes){
          if(planeN > fidAllPlanes.size()){
            std::cerr << " Error: Somehow the current plane iterator exceeds the number of planes in the detector: " << std::endl;
            std::cerr << " Iterator: " << planeN << " of " << allPlanes.size() << " total possible planes " << std::endl;
            std::exit(1);
          } // Debug
          // Check if this is the plane it (likely) entered the detector through 
          // Determine a maximum allowed distance from the plane to count it as the entrance
          if(enteringPlane.GetLabel() == pl.GetLabel()){
            if(distFromEntrance < 1){
              nPlanesCrossed++;
              labelsCrossed.push_back(pl.GetLabel());
            }
          }
          else if(exitingPlane.GetLabel() == pl.GetLabel()){
            if(distFromExit < 1){
              nPlanesCrossed++;
              labelsCrossed.push_back(pl.GetLabel());
            }
          }
          // Otherwise, check if the track intersects the current plane
          else if(CheckIfIntersectsPlane(pl,startVtx,endVtx,length)){
            nPlanesCrossed++;
            labelsCrossed.push_back(pl.GetLabel());
          } // Intersects
          planeN++;
        } // Planes

        for(const Plane &pl : fidExtPlanes){
          if(enteringPlane.GetLabel() == pl.GetLabel()){
            if(distFromEntrance < 1){
              nExtCrossed++;
            }
          } // Intersects
          else if(exitingPlane.GetLabel() == pl.GetLabel()){
            if(distFromExit < 1){
              nExtCrossed++;
            }
          } // Intersects
          else if(CheckIfIntersectsPlane(pl,startVtx,endVtx,length)){
            nExtCrossed++;
          } // Intersects
        } // Planes
        // Now count the planes crossed for the stats table
        bool thruGoing = false;
        if(nExtCrossed == 1){
          stopping++;
        }
        if(nExtCrossed >= 2){
          thruGoing = true;
          exiting++;
        }

        // Loop over the labels crossed by this track and fill the appropriate counters
        bool APA = false;
        bool CPA = false;
        bool top = false;
        bool bot = false;

        for(std::string &str : labelsCrossed){
          // Get the converted label
          std::string longLab = planeLabels.find(str)->second;
          TString lab(longLab);
          if(lab.Contains("APA"))
            APA = true;
          else if(lab.Contains("CPA"))
            CPA = true;
          else if(lab.Contains("Top"))
            top = true;
          else if(lab.Contains("Bot"))
            bot = true;
        }
        if(APA || CPA)
          min1APACPA++;
        if(APA && CPA)
          min2APACPA++;
        if(top || bot)
          topOrBottom++;
        if(top && bot){
          topBottom++;
        }
        // Length cuts (2m)
        if(!evtProc.SelectTrack(evt,iTrk)) continue;
        nLongTracks++;

        if(nExtCrossed == 1){
          nStoppingLong++;
        }
        if(startVtx.Y() < 599.5) continue; // Make sure the tracks start at the top of the detector
        nLongHighYTracks++;

      } // Track loop
    }// Event loop

    // Sort out the counter maps
    prodCounters["Events"].at(f) = nEvts; 
    prodCounters["Tracks"].at(f) = totalTracks;
    prodCounters["Muons"].at(f) = nMu;
    prodCounters["Crosses top or bottom"].at(f) = topOrBottom;
    prodCounters["Crosses top and bottom"].at(f) = topBottom;
    prodCounters["Crosses $ \\geq $ 1 APA/CPA"].at(f) = min1APACPA;
    prodCounters["Crosses $ \\geq $ 2 APA/CPA"].at(f) = min2APACPA;
    prodCounters["Stopping"].at(f) = stopping;
    prodCounters["Exiting"].at(f) = exiting;
    prodCounters["$\\mu > 3$~m"].at(f) = nLongTracks;
    prodCounters["Stopping \\& $\\mu > 3$~m"].at(f) = nStoppingLong;
    prodCounters["$\\mu > 3$~m \\& $y_{i} > 599.5$~cm"].at(f) =nLongHighYTracks;

    f++;
  }// File loop
  std::cout << " --- 100 % --- |" << std::endl;

  ofstream texFile;
  texFile.open(location+"/multiprod_contents"+tag+".tex");
  WriteStatsToTeXMultiProd(texFile, nEvents, prodCounters, counterLabels, prodTeXLabels, verbose);

  // End of script
  std::cout << " ...finished analysis" << std::endl;
  std::cout << "-----------------------------------------------------------" << std::endl;
  time_t rawtime_end;
  GetTime(rawtime_end);
  std::cout << "-----------------------------------------------------------" << std::endl;
  GetTotalTime(rawtime, rawtime_end);
  std::cout << "-----------------------------------------------------------" << std::endl;
  
  return 0;
}