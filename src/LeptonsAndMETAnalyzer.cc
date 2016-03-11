////////////////////////////////////////////////////////////////////////////////
//
// LeptonsAndMETAnalyzer
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

#include "JMEAnalysis/JMEValidator/interface/LeptonsAndMETAnalyzer.h"

#include <vector>

#include <math.h>

////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
LeptonsAndMETAnalyzer::LeptonsAndMETAnalyzer(const edm::ParameterSet& iConfig)
    : JME::Analyzer(iConfig)
    , srcIsoMuons_   (consumes<edm::View<reco::Candidate>>(iConfig.getParameter<edm::InputTag>("srcIsoMuons")))
    , srcMET_        (consumes<std::vector<pat::MET>>(iConfig.getParameter<edm::InputTag>("srcMET")))
    , srcPUPPET_     (consumes<std::vector<reco::PFMET>>(iConfig.getParameter<edm::InputTag>("srcPUPPET")))
    , srcVtx_        (iConfig.getParameter<edm::InputTag> ("srcVtx"))
    , srcMuons_      (iConfig.getParameter<edm::InputTag> ("srcMuons"))
    , srcVMCHSTAND_  (iConfig.getParameter<edm::InputTag> ("srcVMCHSTAND"))
    , srcVMNHSTAND_  (iConfig.getParameter<edm::InputTag> ("srcVMNHSTAND"))
    , srcVMPhSTAND_  (iConfig.getParameter<edm::InputTag> ("srcVMPhSTAND"))
    , srcVMPUSTAND_  (iConfig.getParameter<edm::InputTag> ("srcVMPUSTAND"))
    , srcVMNHPFWGT_  (iConfig.getParameter<edm::InputTag> ("srcVMNHPFWGT"))
    , srcVMPhPFWGT_  (iConfig.getParameter<edm::InputTag> ("srcVMPhPFWGT"))
    , srcVMCHPUPPI_  (iConfig.getParameter<edm::InputTag> ("srcVMCHPUPPI"))
    , srcVMNHPUPPI_  (iConfig.getParameter<edm::InputTag> ("srcVMNHPUPPI"))
    , srcVMPhPUPPI_  (iConfig.getParameter<edm::InputTag> ("srcVMPhPUPPI"))
    , srcVMCHNOMUONPUPPI_  (iConfig.getParameter<edm::InputTag> ("srcVMCHNOMUONPUPPI"))
    , srcVMNHNOMUONPUPPI_  (iConfig.getParameter<edm::InputTag> ("srcVMNHNOMUONPUPPI"))
    , srcVMPhNOMUONPUPPI_  (iConfig.getParameter<edm::InputTag> ("srcVMPhNOMUONPUPPI"))
{


}


//______________________________________________________________________________
LeptonsAndMETAnalyzer::~LeptonsAndMETAnalyzer()
{

}


////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void LeptonsAndMETAnalyzer::beginJob()
{
  JME::Analyzer::beginJob();
  eventCounter_ = 0;
}


//______________________________________________________________________________
void LeptonsAndMETAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  edm::Handle<edm::View<reco::Candidate> >           muonsForZ;
  edm::Handle<std::vector<pat::MET> >            mets;
  edm::Handle<std::vector<reco::PFMET> >         puppet;

  //EVENT INFORMATION
  run = iEvent.id().run();
  lumiBlock = iEvent.id().luminosityBlock();
  event = iEvent.id().event();

  iEvent.getByToken(srcMET_,mets);
  assert(mets.isValid());
  const pat::MET  &inPFMET = mets.product()->front();
  pfMET      = inPFMET.pt();
  pfMETphi   = inPFMET.phi();

  iEvent.getByToken(srcPUPPET_,puppet);
  assert(mets.isValid());
  const reco::PFMET &inPuppET = puppet.product()->front();
  puppET      = inPuppET.pt();
  puppETphi   = inPuppET.phi();

  iEvent.getByToken(srcIsoMuons_, muonsForZ);
  // std::cout << muonsForZ->size() << std::endl;
  int nZ = 0;
  TLorentzVector theZCand;
  for(size_t i = 0, n = muonsForZ->size(); i < n; ++i) {
    edm::Ptr<reco::Candidate> muPtr1 = muonsForZ->ptrAt(i);
    for(size_t j = 1, n = muonsForZ->size(); j < n; ++j) {
      edm::Ptr<reco::Candidate> muPtr2 = muonsForZ->ptrAt(j);
      if (muPtr1->charge()*muPtr2->charge() < 1){
        // std::cout << " oppo " << std::endl;
        TLorentzVector Zcand = TLorentzVector(muPtr1->px(),muPtr1->py(),muPtr1->pz(),muPtr1->energy()) + TLorentzVector(muPtr2->px(),muPtr2->py(),muPtr2->pz(),muPtr2->energy());
        float Zmass = Zcand.M();
        // std::cout << "Zmass = " << Zmass << std::endl;
        float Zdiff = fabs(Zmass - 91.2);
        if (Zdiff < 15){
          nZ += 1;
          theZCand = Zcand;
        }
      }
    }
  }
  

  if (nZ > 0){
    nZcands = nZ;
    ZpT = theZCand.Pt();
    Zphi = theZCand.Phi();
    Zmass = theZCand.M();
    pfMET_uPara = 0.;
    pfMET_uPerp = 0.;
    puppET_uPara = 0.;
    puppET_uPerp = 0.;
    recoilComputation( pfMET, pfMETphi, ZpT, Zphi, pfMET_uPara, pfMET_uPerp);
    recoilComputation( puppET, puppETphi, ZpT, Zphi, puppET_uPara, puppET_uPerp);
  }
  else{
    nZcands = 0;
    ZpT = -1;
    Zphi = -1;
    Zmass = -1;
    pfMET_uPara = -1;
    pfMET_uPerp = -1;
    puppET_uPara = -1;
    puppET_uPerp = -1;
  }


  // - - - - - - - - - - - - - - - - - 
  // - - Muon Isolation Calculation - -
  // - - - - - - - - - - - - - - - - - 

  npv = 0 ; 
  mupt             .clear();
  mueta 	  .clear();
  muphi 	  .clear();
  mue   	  .clear();
  muIsoRAW   	  .clear();
  muIsoSTAND 	  .clear();
  muIsoPFWGT 	  .clear();
  muIsoPUPPI 	  .clear();
  muIsoNOMUONPUPPI.clear();
  muIso_CH     	  .clear();
  muIso_NU     	  .clear();
  muIso_PH     	  .clear();
  muIso_PU     	  .clear();
  muIso_NUPFW  	  .clear();
  muIso_PHPFW  	  .clear();
  muIso_CHPUPPI	  .clear();
  muIso_NUPUPPI	  .clear();
  muIso_PHPUPPI	  .clear();
  muIso_CHNOMUONPUPPI.clear();
  muIso_NUNOMUONPUPPI.clear();
  muIso_PHNOMUONPUPPI.clear();

  edm::Handle<edm::View<pat::Muon> >  muons;
  edm::Handle<edm::ValueMap<double> > VMCHSTAND   ;
  edm::Handle<edm::ValueMap<double> > VMNHSTAND   ;
  edm::Handle<edm::ValueMap<double> > VMPhSTAND   ;
  edm::Handle<edm::ValueMap<double> > VMPUSTAND   ;
  edm::Handle<edm::ValueMap<double> > VMNHPFWGT   ;
  edm::Handle<edm::ValueMap<double> > VMPhPFWGT   ;
  edm::Handle<edm::ValueMap<double> > VMCHPUPPI   ;
  edm::Handle<edm::ValueMap<double> > VMNHPUPPI   ;
  edm::Handle<edm::ValueMap<double> > VMPhPUPPI   ;
  edm::Handle<edm::ValueMap<double> > VMCHNOMUONPUPPI   ;
  edm::Handle<edm::ValueMap<double> > VMNHNOMUONPUPPI   ;
  edm::Handle<edm::ValueMap<double> > VMPhNOMUONPUPPI   ;
  edm::Handle<std::vector<reco::Vertex> >        vtx;

  iEvent.getByLabel(srcVtx_,vtx ); 
  const reco::VertexCollection::const_iterator vtxEnd = vtx->end();
  for (reco::VertexCollection::const_iterator vtxIter = vtx->begin(); vtxEnd != vtxIter; ++vtxIter) {
    if (!vtxIter->isFake() && vtxIter->ndof()>=4 && fabs(vtxIter->z())<=24)
      npv++;
  }

  iEvent.getByLabel(srcMuons_, muons);
  iEvent.getByLabel(srcVMNHPFWGT_, VMNHPFWGT);
  iEvent.getByLabel(srcVMPhPFWGT_, VMPhPFWGT);
  iEvent.getByLabel(srcVMCHPUPPI_, VMCHPUPPI);
  iEvent.getByLabel(srcVMNHPUPPI_, VMNHPUPPI);
  iEvent.getByLabel(srcVMPhPUPPI_, VMPhPUPPI);
  iEvent.getByLabel(srcVMCHSTAND_, VMCHSTAND);
  iEvent.getByLabel(srcVMNHSTAND_, VMNHSTAND);
  iEvent.getByLabel(srcVMPhSTAND_, VMPhSTAND);
  iEvent.getByLabel(srcVMPUSTAND_, VMPUSTAND);
  iEvent.getByLabel(srcVMCHNOMUONPUPPI_, VMCHNOMUONPUPPI);
  iEvent.getByLabel(srcVMNHNOMUONPUPPI_, VMNHNOMUONPUPPI);
  iEvent.getByLabel(srcVMPhNOMUONPUPPI_, VMPhNOMUONPUPPI);

  for(size_t i = 0, n = muons->size(); i < n; ++i) {
    edm::Ptr<pat::Muon> muPtr = muons->ptrAt(i);

    // - - - - - - - - - - - - - - - - 
    // - - - - Require tight-ID - - - -
    // - - - - - - - - - - - - - - - - 
    if( ! muPtr->isPFMuon() ) continue ;
    if( ! muPtr->isGlobalMuon() ) continue ; 
    if( abs(muPtr->muonBestTrack()->dxy(vtx->at(0).position()) ) > 0.2 ) continue ;
    if( abs(muPtr->muonBestTrack()->dz (vtx->at(0).position()) ) > 0.5 ) continue ; 
    if( muPtr->globalTrack()->normalizedChi2() > 10. ) continue ;
    if( muPtr->globalTrack()->hitPattern().numberOfValidMuonHits() == 0 ) continue ;
    if( muPtr->innerTrack()->hitPattern().numberOfValidPixelHits() == 0 ) continue ;
    if( muPtr->track()->hitPattern().trackerLayersWithMeasurement() <= 5 ) continue ;
    if( muPtr->numberOfMatchedStations() <= 1 ) continue ;
    // - - - - - - - - - - - - - - - - 
    // - - - - end of requirement - - - - 
    // - - - - - - - - - - - - - - - - 

    mupt  . push_back( muPtr->pt());
    mueta . push_back( muPtr->eta());
    muphi . push_back( muPtr->phi());
    mue   . push_back( muPtr->energy());

    //Raw Isolation
    //I = [sumChargedHadronPt+ max(0.,sumNeutralHadronPt+sumPhotonPt]/pt
    muIsoRAW . push_back ( ((*VMCHSTAND)[muPtr] + std::max(0.0,(*VMNHSTAND)[muPtr]+(*VMPhSTAND)[muPtr]))/muPtr->pt() ) ;
    
    //Delta Beta (see https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Muon_Isolation for more details)
    //I = [sumChargedHadronPt+ max(0.,sumNeutralHadronPt+sumPhotonPt-0.5sumPUPt]/pt
    muIsoSTAND . push_back( ((*VMCHSTAND)[muPtr] + std::max(0.0,(*VMNHSTAND)[muPtr]+(*VMPhSTAND)[muPtr]-(0.5*(*VMPUSTAND)[muPtr] ) ) )/muPtr->pt() );

    // PF Weighted (see https://twiki.cern.ch/twiki/bin/viewauth/CMS/MuonIsolationForRun2 for more details)
    muIsoPFWGT . push_back( ((*VMCHSTAND)[muPtr]+(*VMNHPFWGT)[muPtr]+(*VMPhPFWGT)[muPtr])/muPtr->pt() );
    
    // PUPPI Weighted (see https://twiki.cern.ch/twiki/bin/viewauth/CMS/MuonIsolationForRun2 for more details)
    muIsoPUPPI . push_back( ((*VMCHPUPPI)[muPtr]+(*VMNHPUPPI)[muPtr]+(*VMPhPUPPI)[muPtr])/muPtr->pt() );

    // PUPPI Weighted without muons
    muIsoNOMUONPUPPI . push_back( ((*VMCHNOMUONPUPPI)[muPtr]+(*VMNHNOMUONPUPPI)[muPtr]+(*VMPhNOMUONPUPPI)[muPtr])/muPtr->pt() );

    muIso_CH   .push_back( (*VMCHSTAND)[muPtr] );
    muIso_NU   .push_back( (*VMNHSTAND)[muPtr] );
    muIso_PH   .push_back( (*VMPhSTAND)[muPtr] );
    muIso_PU   .push_back( (*VMPUSTAND)[muPtr] );
    muIso_NUPFW  .push_back( (*VMNHPFWGT)[muPtr] );
    muIso_PHPFW  .push_back( (*VMPhPFWGT)[muPtr] );
    muIso_CHPUPPI .push_back( (*VMCHPUPPI)[muPtr] );
    muIso_NUPUPPI .push_back( (*VMNHPUPPI)[muPtr] );
    muIso_PHPUPPI .push_back( (*VMPhPUPPI)[muPtr] );
    muIso_CHNOMUONPUPPI .push_back( (*VMCHNOMUONPUPPI)[muPtr] );
    muIso_NUNOMUONPUPPI .push_back( (*VMNHNOMUONPUPPI)[muPtr] );
    muIso_PHNOMUONPUPPI .push_back( (*VMPhNOMUONPUPPI)[muPtr] );

  }




  tree.fill();  
  eventCounter_++;
}

void LeptonsAndMETAnalyzer::recoilComputation( float &met, float &metPhi, float &Zpt, float &Zphi, float &upara, float &uperp){
  
  double p1UX  = met*cos(metPhi) + Zpt*cos(Zphi);
  double p1UY  = met*sin(metPhi) + Zpt*sin(Zphi);
  double p1U   = sqrt(p1UX*p1UX+p1UY*p1UY);
  double p1Cos = - (p1UX*cos(Zphi) + p1UY*sin(Zphi))/p1U;
  double p1Sin =   (p1UX*sin(Zphi) - p1UY*cos(Zphi))/p1U;
  upara   = p1U*p1Cos;//*(pU1*(iGenPt > 10) + (iGenPt > 10)*((1.-iGenPt/10.)*(pU1-1.)+1.));
  uperp   = p1U*p1Sin;

}
