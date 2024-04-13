void DocumentSourceUnionWith::doDispose() {
    if (_pipeline) {
        _stats.planSummaryStats.usedDisk =
            _stats.planSummaryStats.usedDisk || _pipeline->usedDisk();
        recordPlanSummaryStats(*_pipeline);

        if (!_pipeline->getContext()->explain) {
            _pipeline->dispose(pExpCtx->opCtx);
            _pipeline.reset();
        }
    }
}