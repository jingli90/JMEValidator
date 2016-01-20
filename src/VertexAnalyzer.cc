#include "JMEAnalysis/JMEValidator/interface/VertexAnalyzer.h"

VertexAnalyzer::VertexAnalyzer(const edm::ParameterSet& iConfig): JME::Analyzer(iConfig),
    srcToken_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("src")))
{
    // Empty
}

VertexAnalyzer::~VertexAnalyzer() {
    // Empty
}

void VertexAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

    edm::Handle<reco::VertexCollection> verticesHandle;
    iEvent.getByToken(srcToken_, verticesHandle);
    const reco::VertexCollection& vertices = *verticesHandle;

    for (const reco::Vertex& vertex: vertices) {
        ndof_.push_back(vertex.ndof());
        normalizedChi2_.push_back(vertex.normalizedChi2());
        isFake_.push_back(vertex.isFake());
        isValid_.push_back(vertex.isValid());
        position_.push_back(vertex.position());
        covariance_.push_back(vertex.covariance());
    }

    tree.fill();
}
