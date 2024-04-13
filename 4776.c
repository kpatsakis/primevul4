DocumentSourceUnionWith::~DocumentSourceUnionWith() {
    if (_pipeline && _pipeline->getContext()->explain) {
        _pipeline->dispose(pExpCtx->opCtx);
        _pipeline.reset();
    }
}