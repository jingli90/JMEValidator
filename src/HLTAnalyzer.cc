#include "JMEAnalysis/JMEValidator/interface/HLTAnalyzer.h"

HLTAnalyzer::HLTAnalyzer(const edm::ParameterSet& iConfig): JME::Analyzer(iConfig),
    triggerResultsToken_(consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("src"))),
    triggerPrescalesToken_(consumes<pat::PackedTriggerPrescales>(iConfig.getParameter<edm::InputTag>("prescales"))),
    triggerObjectsToken_(consumes<pat::TriggerObjectStandAloneCollection>(iConfig.getParameter<edm::InputTag>("objects")))
{
    // Empty
}

HLTAnalyzer::~HLTAnalyzer() {
    // Empty
}

void HLTAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

    edm::Handle<edm::TriggerResults> triggerResults;
    edm::Handle<pat::PackedTriggerPrescales> triggerPrescales;
    edm::Handle<pat::TriggerObjectStandAloneCollection> triggerObjects;

    iEvent.getByToken(triggerResultsToken_, triggerResults);
    iEvent.getByToken(triggerPrescalesToken_, triggerPrescales);
    iEvent.getByToken(triggerObjectsToken_, triggerObjects);

    const edm::TriggerNames& triggerNames = iEvent.triggerNames(*triggerResults);

    for (size_t i = 0 ; i < triggerResults->size(); i++) {
        if (triggerResults->accept(i)) {
            std::string triggerName = triggerNames.triggerName(i);
            if (triggerName == "HLTriggerFinalPath")
                continue; // This one is pretty useless...
            if (triggerName[0] == 'A')
                continue; // Remove AlCa HLT paths

            paths.push_back(triggerName);
            if (triggerPrescales.isValid()) {
                prescales.push_back(triggerPrescales->getPrescaleForIndex(i));
            }
        }
    }

    for (pat::TriggerObjectStandAlone obj : *triggerObjects) { // note: not "const &" since we want to call unpackPathNames
        obj.unpackPathNames(triggerNames);

        objects_pt.push_back(obj.pt());
        objects_eta.push_back(obj.eta());
        objects_phi.push_back(obj.phi());
        objects_e.push_back(obj.energy());

        std::vector<std::string> pathNamesAll  = obj.pathNames(false);

        std::vector<int> paths_indexes;
        std::vector<bool> isl3;
        std::vector<bool> islast;
        for (size_t h = 0, n = pathNamesAll.size(); h < n; ++h) {

            const std::string& pathName = pathNamesAll[h];

            paths_indexes.push_back(std::find(paths.begin(), paths.end(), pathName) - paths.begin());

            bool isBoth = obj.hasPathName(pathName, true, true );
            bool isL3   = obj.hasPathName(pathName, false, true );
            bool isLF   = obj.hasPathName(pathName, true, false );

            if (isBoth) {
                isl3.push_back(true);
                islast.push_back(true);
            } else if (isL3) {
                isl3.push_back(true);
                islast.push_back(false);
            } else if (isLF) {
                isl3.push_back(false);
                islast.push_back(true);
            } else {
                isl3.push_back(false);
                islast.push_back(false);

            }
        }

        objects_paths.push_back(paths_indexes);
        objects_paths_isl3.push_back(isl3);
        objects_paths_islast.push_back(islast);
    }

    tree.fill();
}
