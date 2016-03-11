#pragma once

#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"
#include "RecoEgamma/EgammaTools/interface/EffectiveAreas.h"

#include "JMEAnalysis/JMEValidator/interface/PhysicsObjectAnalyzer.h"


class PhotonAnalyzer: public JME::PhysicsObjectAnalyzer {
    public:
        explicit PhotonAnalyzer(const edm::ParameterSet& iConfig);
        virtual ~PhotonAnalyzer();

        virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) override;

    private:
        edm::EDGetTokenT<pat::PhotonCollection> photons_;
        edm::EDGetTokenT<pat::ElectronCollection> electrons_;
        //edm::EDGetTokenT<reco::VertexCollection> vertices_;
        edm::EDGetTokenT<reco::ConversionCollection> conversions_;
        edm::EDGetTokenT<reco::BeamSpot> beamspot_;
        edm::EDGetTokenT<double> rhoToken_;
        edm::EDGetTokenT<edm::ValueMap<float>> phoChargedHadronIsolationToken_; 
        edm::EDGetTokenT<edm::ValueMap<float>> phoNeutralHadronIsolationToken_; 
        edm::EDGetTokenT<edm::ValueMap<float>> phoPhotonIsolationToken_;
        std::vector<std::pair<std::string, edm::EDGetTokenT<edm::ValueMap<bool>>>> idTokens_;

        EffectiveAreas effAreaChHadrons_;
        EffectiveAreas effAreaNeuHadrons_;
        EffectiveAreas effAreaPhotons_;

    private:
        std::vector<float>& supercluster_eta = tree["supercluster_eta"].write<std::vector<float>>();
        std::vector<float>& supercluster_phi = tree["supercluster_phi"].write<std::vector<float>>();
        std::vector<float>& hOverE = tree["hOverE"].write<std::vector<float>>();
        std::vector<bool>& hasPixelSeed = tree["has_pixel_seed"].write<std::vector<bool>>();
        std::vector<float>& full5x5_sigmaIetaIeta = tree["full5x5_sigmaIetaIeta"].write<std::vector<float>>();
        std::vector<bool>& hasMatchedPromptElectron = tree["has_matched_prompt_electron"].write<std::vector<bool>>();

        // Isolation
        std::vector<float>& chargedHadronIsoR03_ = tree["chargedHadronIsoR03"].write<std::vector<float>>();
        std::vector<float>& neutralHadronIsoR03_ = tree["neutralHadronIsoR03"].write<std::vector<float>>();
        std::vector<float>& photonIsoR03_ = tree["photonIsoR03"].write<std::vector<float>>();

        std::vector<float>& chargedHadronIsoR03EA_ = tree["chargedHadronIsoR03_withEA"].write<std::vector<float>>();
        std::vector<float>& neutralHadronIsoR03EA_ = tree["neutralHadronIsoR03_withEA"].write<std::vector<float>>();
        std::vector<float>& photonIsoR03EA_ = tree["photonIsoR03_withEA"].write<std::vector<float>>();

        std::vector<std::map<std::string, bool>>& ids_ = tree["ids"].write<std::vector<std::map<std::string, bool>>>();
};

