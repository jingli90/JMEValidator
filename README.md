JMEValidator
======

Common framework for CMS JEC / JER analyzes.

### Recipe

```sh
export SCRAM_ARCH=slc6_amd64_gcc493
cmsrel CMSSW_7_6_3_patch1
cd CMSSW_7_6_3_patch1/src/
cmsenv

git cms-init

# Framework
git clone git@github.com:cms-jet/JetToolbox.git JMEAnalysis/JetToolbox -b jetToolbox_763
git clone git@github.com:blinkseb/TreeWrapper.git JMEAnalysis/TreeWrapper
git clone git@github.com:cms-jet/JMEValidator.git JMEAnalysis/JMEValidator -b CMSSW_7_6_X

scram b -j8

cd JMEAnalysis/JMEValidator/test
cmsRun runFrameworkMC.py
```
