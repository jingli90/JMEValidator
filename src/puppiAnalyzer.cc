////////////////////////////////////////////////////////////////////////////////
//
// puppiAnalyzer
// -----------
//
//                        01/07/2014 Alexx Perloff   <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
 
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefToBase.h"
#include "DataFormats/Common/interface/RefVector.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/CandMatchMap.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/JetCollection.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/Common/interface/View.h"

#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"

#include "SimDataFormats/JetMatching/interface/JetMatchedPartons.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "JMEAnalysis/JMEValidator/interface/puppiAnalyzer.h"

#include <vector>

////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
puppiAnalyzer::puppiAnalyzer(const edm::ParameterSet& iConfig)
    : JME::Analyzer(iConfig)
{
  eventLimit_     = iConfig.getParameter<int>("maxEvents");
    
  nAlgosToken_ = consumes<double>(iConfig.getParameter<edm::InputTag>("nAlgos"));
  rawAlphasToken_ = consumes<std::vector<double>>(iConfig.getParameter<edm::InputTag>("rawAlphas"));
  alphasToken_ = consumes<std::vector<double>>(iConfig.getParameter<edm::InputTag>("alphas"));
  alphasMedToken_ = consumes<std::vector<double>>(iConfig.getParameter<edm::InputTag>("alphasMed"));
  alphasRmsToken_ = consumes<std::vector<double>>(iConfig.getParameter<edm::InputTag>("alphasRms"));
  packedPFCandidatesToken_ = consumes<reco::CandidateView>(iConfig.getParameter<edm::InputTag>("packedPFCandidates"));
}


//______________________________________________________________________________
puppiAnalyzer::~puppiAnalyzer()
{

}


////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void puppiAnalyzer::beginJob()
{
  JME::Analyzer::beginJob();
  eventCounter_ = 0;
}


//______________________________________________________________________________
void puppiAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  if (eventCounter_ > eventLimit_) return;

  edm::Handle<double> nalgosHandle;
  iEvent.getByToken(nAlgosToken_, nalgosHandle);

  edm::Handle<std::vector<double>> rawAlphas;
  iEvent.getByToken(rawAlphasToken_, rawAlphas);

  edm::Handle<std::vector<double>> TheAlphas;
  iEvent.getByToken(alphasToken_, TheAlphas);

  edm::Handle<std::vector<double>> TheAlphasMed;
  iEvent.getByToken(alphasMedToken_, TheAlphasMed);

  edm::Handle<std::vector<double>> TheAlphasRms;
  iEvent.getByToken(alphasRmsToken_, TheAlphasRms);

  edm::Handle<reco::CandidateView> hPFProduct;
  iEvent.getByToken(packedPFCandidatesToken_, hPFProduct);
  const reco::CandidateView *pfCol = hPFProduct.product();

  nalgos = *nalgosHandle;

  int ctr = 0;
  for(reco::CandidateView::const_iterator itPF = pfCol->begin(); itPF!=pfCol->end(); itPF++) {
    px.push_back( itPF->px() );
    py.push_back( itPF->py() );
    pz.push_back( itPF->pz() );
    e.push_back( itPF->energy() );
    id.push_back( float(itPF->pdgId()) );
    charge.push_back( float(itPF->charge()) );

    thealphas.push_back( (*TheAlphas)[ctr] );
    thealphasmed.push_back( (*TheAlphasMed)[ctr] );
    thealphasrms.push_back( (*TheAlphasRms)[ctr] );
    
    const pat::PackedCandidate *lPack = dynamic_cast<const pat::PackedCandidate*>(&(*itPF));
    fromPV.push_back( float(lPack->fromPV()) );
    ctr++;
  }


  for(unsigned int i = 0; i < rawAlphas->size(); i++){
    alphas.push_back( (*rawAlphas)[i] );
  }

  tree.fill();
  
  eventCounter_++;
}
