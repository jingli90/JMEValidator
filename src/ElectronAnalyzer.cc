#include "JMEAnalysis/JMEValidator/interface/ElectronAnalyzer.h"

#include <DataFormats/RecoCandidate/interface/IsoDeposit.h>
#include <DataFormats/RecoCandidate/interface/IsoDepositVetos.h>

ElectronAnalyzer::ElectronAnalyzer(const edm::ParameterSet& iConfig): JME::LeptonAnalyzer(iConfig),
    electrons_(consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("src"))),
    vertices_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"))),
    conversions_(consumes<reco::ConversionCollection>(iConfig.getParameter<edm::InputTag>("conversions"))),
    beamspot_(consumes<reco::BeamSpot>(iConfig.getParameter<edm::InputTag>("beamspot"))),
    rhoToken_(consumes<double>(iConfig.getParameter<edm::InputTag>("rho")))
{
    if (iConfig.existsAs<std::vector<edm::InputTag>>("ids")) {
        const std::vector<edm::InputTag>& ids = iConfig.getParameter<std::vector<edm::InputTag>>("ids");

        for (const edm::InputTag& id: ids) {
            idTokens_.push_back(std::make_pair(id.instance(), consumes<edm::ValueMap<bool>>(id)));
        }
    }
}

ElectronAnalyzer::~ElectronAnalyzer() {
    // Empty
}

void ElectronAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

    edm::Handle<pat::ElectronCollection> electronsHandle;
    iEvent.getByToken(electrons_, electronsHandle);

    edm::Handle<reco::VertexCollection> verticesHandle;
    iEvent.getByToken(vertices_, verticesHandle);
    const auto& primaryVertex = verticesHandle->at(0);

    edm::Handle<reco::ConversionCollection> conversionsHandle;
    iEvent.getByToken(conversions_, conversionsHandle);

    edm::Handle<reco::BeamSpot> beamSpotHandle;
    iEvent.getByToken(beamspot_, beamSpotHandle);

    edm::Handle<double> rhoHandle;
    iEvent.getByToken(rhoToken_, rhoHandle);
    double rho = *rhoHandle;

    std::vector<std::pair<std::string, edm::Handle<edm::ValueMap<bool>>>> idHandles;
    for (auto& token: idTokens_) {
        edm::Handle<edm::ValueMap<bool>> idHandle;
        iEvent.getByToken(token.second, idHandle);
        idHandles.push_back(std::make_pair(token.first, idHandle));
    }

    // Loop over electrons
    size_t index = 0;
    for (const pat::Electron& electron: *electronsHandle) {
        extractBasicProperties(electron);
        extractGenProperties(electron.genLepton());

        supercluster_eta.push_back(electron.superCluster()->eta());
        supercluster_phi.push_back(electron.superCluster()->phi());

        dEtaIn.push_back(electron.deltaEtaSuperClusterTrackAtVtx());
        dPhiIn.push_back(electron.deltaPhiSuperClusterTrackAtVtx());
        hOverE.push_back(electron.hcalOverEcal());
        full5x5_sigmaIetaIeta.push_back(electron.full5x5_sigmaIetaIeta());
        // |1/E-1/p| = |1/E - EoverPinner/E| is computed below
        if ((electron.ecalEnergy() == 0) || (!std::isfinite(electron.ecalEnergy()))) 
          ooEmooP.push_back(1e30);
        else
          ooEmooP.push_back(fabs(1.0 / electron.ecalEnergy() - electron.eSuperClusterOverP() / electron.ecalEnergy()));

        reco::GsfElectron::PflowIsolationVariables pfIso = electron.pfIsolationVariables();
        computeRelativeIsolationR03(electron, pfIso.sumChargedHadronPt, pfIso.sumNeutralHadronEt, pfIso.sumPhotonEt, pfIso.sumPUPt, electron.superCluster()->eta(), rho);
        computeRelativeIsolationR04(electron, electron.chargedHadronIso(), electron.neutralHadronIso(), electron.photonIso(), electron.puChargedHadronIso(), electron.superCluster()->eta(), rho);

        // Impact parameter
        reco::GsfTrackRef theTrack = electron.gsfTrack();
        d0.push_back(-1 * theTrack->dxy(primaryVertex.position()));
        dz.push_back(theTrack->dz(primaryVertex.position()));
        expectedMissingInnerHits.push_back(theTrack->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS));

        // Conversion rejection
        passConversionVeto.push_back(!ConversionTools::hasMatchedConversion(electron, conversionsHandle, beamSpotHandle->position()));

        // IDs
        const pat::ElectronRef electronRef(electronsHandle, index++);
        std::map<std::string, bool> ids;
        for (auto& idHandle: idHandles) {
            ids[idHandle.first] = (*(idHandle.second))[electronRef];
        }
        ids_.push_back(ids);
    }

    tree.fill();
}
