void DocumentSourceUnionWith::recordPlanSummaryStats(const Pipeline& pipeline) {
    for (auto&& source : pipeline.getSources()) {
        if (auto specificStats = source->getSpecificStats()) {
            specificStats->accumulate(_stats.planSummaryStats);
        }
    }
}