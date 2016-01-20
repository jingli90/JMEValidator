#include "JMEAnalysis/JMEValidator/interface/PhotonAnalyzer.h"

namespace pat {
    namespace helper {

        // Copy of https://github.com/cms-sw/cmssw/blob/CMSSW_7_4_X/RecoEgamma/EgammaTools/src/ConversionTools.cc
        // but using pat::Electrons instead of gsfElectrons as input
        bool hasMatchedPromptElectron(const reco::SuperClusterRef &sc, const edm::Handle<pat::ElectronCollection> &eleCol, const edm::Handle<reco::ConversionCollection> &convCol, const math::XYZPoint &beamspot, bool allowCkfMatch=true, float lxyMin=2.0, float probMin=1e-6, unsigned int nHitsBeforeVtxMax=0) {

            //check if a given SuperCluster matches to at least one GsfElectron having zero expected inner hits
            //and not matching any conversion in the collection passing the quality cuts

            if (sc.isNull()) return false;

            for (pat::ElectronCollection::const_iterator it = eleCol->begin(); it!=eleCol->end(); ++it) {
                //match electron to supercluster
                if (it->reco::GsfElectron::superCluster()!=sc) continue;

                //check expected inner hits
                if (it->gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS) > 0) continue;

                //check if electron is matching to a conversion
                if (ConversionTools::hasMatchedConversion(*it,convCol,beamspot,allowCkfMatch,lxyMin,probMin,nHitsBeforeVtxMax)) continue;


                return true;
            }

            return false;
        }
    };
};

PhotonAnalyzer::PhotonAnalyzer(const edm::ParameterSet& iConfig): JME::PhysicsObjectAnalyzer(iConfig),
    photons_(consumes<pat::PhotonCollection>(iConfig.getParameter<edm::InputTag>("src"))),
    electrons_(consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electrons"))),
    //vertices_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"))),
    conversions_(consumes<reco::ConversionCollection>(iConfig.getParameter<edm::InputTag>("conversions"))),
    beamspot_(consumes<reco::BeamSpot>(iConfig.getParameter<edm::InputTag>("beamspot"))),
    rhoToken_(consumes<double>(iConfig.getParameter<edm::InputTag>("rho"))),
    phoChargedHadronIsolationToken_(consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("phoChargedHadronIsolation"))),
    phoNeutralHadronIsolationToken_(consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("phoNeutralHadronIsolation"))),
    phoPhotonIsolationToken_(consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("phoPhotonIsolation"))),

    effAreaChHadrons_((iConfig.getParameter<edm::FileInPath>("effAreaChHadFile")).fullPath()),
    effAreaNeuHadrons_((iConfig.getParameter<edm::FileInPath>("effAreaNeuHadFile")).fullPath()),
    effAreaPhotons_((iConfig.getParameter<edm::FileInPath>("effAreaPhoFile")).fullPath())
{
    if (iConfig.existsAs<std::vector<edm::InputTag>>("ids")) {
        const std::vector<edm::InputTag>& ids = iConfig.getParameter<std::vector<edm::InputTag>>("ids");

        for (const edm::InputTag& id: ids) {
            idTokens_.push_back(std::make_pair(id.instance(), consumes<edm::ValueMap<bool>>(id)));
        }
    }
}

PhotonAnalyzer::~PhotonAnalyzer() {
    // Empty
}

void PhotonAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

    edm::Handle<pat::PhotonCollection> photonsHandle;
    iEvent.getByToken(photons_, photonsHandle);

    edm::Handle<pat::ElectronCollection> electronsHandle;
    iEvent.getByToken(electrons_, electronsHandle);

    //edm::Handle<reco::VertexCollection> verticesHandle;
    //iEvent.getByToken(vertices_, verticesHandle);
    //const auto& primaryVertex = verticesHandle->at(0);

    edm::Handle<reco::ConversionCollection> conversionsHandle;
    iEvent.getByToken(conversions_, conversionsHandle);

    edm::Handle<reco::BeamSpot> beamSpotHandle;
    iEvent.getByToken(beamspot_, beamSpotHandle);

    edm::Handle<double> rhoHandle;
    iEvent.getByToken(rhoToken_, rhoHandle);
    double rho = *rhoHandle;


    // Isolation maps
    edm::Handle<edm::ValueMap<float>> phoChargedHadronIsolationMap;
    iEvent.getByToken(phoChargedHadronIsolationToken_, phoChargedHadronIsolationMap);
    edm::Handle<edm::ValueMap<float>> phoNeutralHadronIsolationMap;
    iEvent.getByToken(phoNeutralHadronIsolationToken_, phoNeutralHadronIsolationMap);
    edm::Handle<edm::ValueMap<float>> phoPhotonIsolationMap;
    iEvent.getByToken(phoPhotonIsolationToken_, phoPhotonIsolationMap);

    std::vector<std::pair<std::string, edm::Handle<edm::ValueMap<bool>>>> idHandles;
    for (auto& token: idTokens_) {
        edm::Handle<edm::ValueMap<bool>> idHandle;
        iEvent.getByToken(token.second, idHandle);
        idHandles.push_back(std::make_pair(token.first, idHandle));
    }

    // Loop over photons
    int index = 0;
    for (const pat::Photon& photon: *photonsHandle) {
        extractBasicProperties(photon);
        extractGenProperties(photon.genParticle());
        //computeIsolation(photon);

        pat::PhotonRef photonRef(photonsHandle, index++);

        supercluster_eta.push_back(photon.superCluster()->eta());
        supercluster_phi.push_back(photon.superCluster()->phi());

        hOverE.push_back(photon.hadTowOverEm());
        hasPixelSeed.push_back(photon.hasPixelSeed());
        hasMatchedPromptElectron.push_back(pat::helper::hasMatchedPromptElectron(photon.superCluster(), electronsHandle, conversionsHandle, beamSpotHandle->position()));

        full5x5_sigmaIetaIeta.push_back(photon.full5x5_sigmaIetaIeta());


        // Isolations
        float chIso = (*phoChargedHadronIsolationMap)[photonRef];
        float nhIso = (*phoNeutralHadronIsolationMap)[photonRef];
        float phIso = (*phoPhotonIsolationMap)[photonRef];
        chargedHadronIsoR03_.push_back(chIso);
        neutralHadronIsoR03_.push_back(nhIso);
        photonIsoR03_.push_back(phIso);

        float eta = fabs(photon.superCluster()->eta());
        chargedHadronIsoR03EA_.push_back(std::max(0.0, chIso - rho * effAreaChHadrons_.getEffectiveArea(eta)));
        neutralHadronIsoR03EA_.push_back(std::max(0.0, nhIso - rho * effAreaNeuHadrons_.getEffectiveArea(eta)));
        photonIsoR03EA_.push_back(std::max(0.0, phIso - rho * effAreaPhotons_.getEffectiveArea(eta)));

        // IDs
        std::map<std::string, bool> ids;
        for (auto& idHandle: idHandles) {
            ids[idHandle.first] = (*(idHandle.second))[photonRef];
        }
        ids_.push_back(ids);
    }

    tree.fill();
}
