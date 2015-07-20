from JMEAnalysis.JMEValidator.FrameworkConfiguration import createProcess

import FWCore.ParameterSet.Config as cms

process = createProcess(isMC=True, globalTag="MCRUN2_74_V9", readJECFromDB=True, jec_database='PY8_RunIISpring15DR74_bx25_MC.db', jec_db_prefix='PY8_RunIISpring15DR74_bx25_MC')

process.source.fileNames = cms.untracked.vstring(
        '/store/mc/RunIISpring15DR74/QCD_Pt-15to7000_TuneCUETP8M1_Flat_13TeV_pythia8/MINIAODSIM/Asympt25nsRaw_MCRUN2_74_V9-v3/00000/4E427174-8612-E511-8F35-1CC1DE1CE56C.root'
    )

