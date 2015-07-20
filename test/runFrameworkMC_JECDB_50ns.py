from JMEAnalysis.JMEValidator.FrameworkConfiguration import createProcess

import FWCore.ParameterSet.Config as cms

process = createProcess(isMC=True, globalTag="MCRUN2_74_V9A", readJECFromDB=True, jec_database='PY8_RunIISpring15DR74_bx50_MC.db', jec_db_prefix='PY8_RunIISpring15DR74_bx50_MC')

process.source.fileNames = cms.untracked.vstring(
        '/store/mc/RunIISpring15DR74/QCD_Pt-15to7000_TuneCUETP8M1_Flat_13TeV_pythia8/MINIAODSIM/Asympt50nsRaw_MCRUN2_74_V9A-v3/00000/DCED9F83-8906-E511-8E6B-0002C90EB9D8.root'
    )

