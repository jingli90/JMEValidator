from JMEAnalysis.JMEValidator.FrameworkConfiguration import createProcess

import FWCore.ParameterSet.Config as cms

process = createProcess(isMC = False, globalTag = "GR_P_V56", readJECFromDB=True, jec_database='PY8_RunIISpring15DR74_bx50_MC.db', jec_db_prefix='PY8_RunIISpring15DR74_bx50_MC')

process.source.fileNames = cms.untracked.vstring(
        '/store/data/Run2015B/ZeroBias/MINIAOD/PromptReco-v1/000/250/987/00000/90874BD2-A925-E511-930E-02163E0133F0.root',
    )

