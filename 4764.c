bool DocumentSourceUnionWith::usedDisk() {
    if (_pipeline) {
        _stats.planSummaryStats.usedDisk =
            _stats.planSummaryStats.usedDisk || _pipeline->usedDisk();
    }
    return _stats.planSummaryStats.usedDisk;
}