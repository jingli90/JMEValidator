from JMEAnalysis.JMEValidator.FrameworkConfiguration import createProcess

import FWCore.ParameterSet.Config as cms

process = createProcess(isMC = False, globalTag = "76X_dataRun2_v15")

process.source.fileNames = cms.untracked.vstring(
        '/store/data/Run2015D/JetHT/MINIAOD/16Dec2015-v1/00000/301A497D-70B0-E511-9630-002590D0AFA8.root'
    )

