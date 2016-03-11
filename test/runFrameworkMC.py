from JMEAnalysis.JMEValidator.FrameworkConfiguration import createProcess

import FWCore.ParameterSet.Config as cms

process = createProcess(isMC = True, globalTag = "76X_mcRun2_asymptotic_v12")

process.source.fileNames = cms.untracked.vstring(
        '/store/mc/RunIIFall15MiniAODv2/QCD_Pt_170to300_TuneCUETP8M1_13TeV_pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/50000/0015AC6D-53B9-E511-8028-0025905C9740.root'
    )

