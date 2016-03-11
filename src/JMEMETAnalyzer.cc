////////////////////////////////////////////////////////////////////////////////
//
// JMEMETAnalyzer
// ------------------
//
//                        05/2015    Sébastien Brochet <sebastien.brochet@cern.ch>
////////////////////////////////////////////////////////////////////////////////

#include "JMEAnalysis/JMEValidator/interface/JMEMETAnalyzer.h"

#include <vector>
#include <iostream>
#include <regex>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
JMEMETAnalyzer::JMEMETAnalyzer(const edm::ParameterSet& iConfig): JME::PhysicsObjectAnalyzer(iConfig),
    src_(consumes<std::vector<pat::MET>>(iConfig.getParameter<edm::InputTag>("src")))
{
    if (iConfig.existsAs<edm::InputTag>("caloMET"))
        caloMETToken_ = consumes<std::vector<pat::MET>>(iConfig.getParameter<edm::InputTag>("caloMET"));
}


//______________________________________________________________________________
JMEMETAnalyzer::~JMEMETAnalyzer()
{

}


////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________

//______________________________________________________________________________
void JMEMETAnalyzer::analyze(const edm::Event& iEvent,
        const edm::EventSetup& iSetup)
{

    edm::Handle<std::vector<pat::MET>> metHandle;
    iEvent.getByToken(src_, metHandle);

    const pat::MET& met = metHandle->at(0);

    extractBasicProperties(met);
    sumEt.push_back(met.sumEt());
    significance.push_back(met.significance());

    uncorrectedPt.push_back(met.uncorPt());
    uncorrectedPhi.push_back(met.uncorPhi());
    uncorrectedSumEt.push_back(met.uncorSumEt());

    extractGenProperties(met.genMET());
    if (met.genMET()) {
        gen_sumEt.push_back(met.genMET()->sumEt());
        gen_significance.push_back(met.genMET()->significance());
    } else {
        gen_sumEt.push_back(0);
        gen_significance.push_back(0);
    }

    if (!caloMETToken_.isUninitialized()) {

        edm::Handle<std::vector<pat::MET>> caloMETHandle;
        iEvent.getByToken(caloMETToken_, caloMETHandle);
        const pat::MET& caloMet = caloMETHandle->at(0);
        // FIXME: Calo met will be removed soon
        caloMET_pt.push_back(caloMet.caloMETPt());
        caloMET_phi.push_back(caloMet.caloMETPhi());
        caloMET_sumEt.push_back(caloMet.caloMETSumEt());
    }

    tree.fill();
}
