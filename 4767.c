Value DocumentSourceUnionWith::serialize(boost::optional<ExplainOptions::Verbosity> explain) const {
    auto collectionless = _pipeline->getContext()->ns.isCollectionlessAggregateNS();
    if (explain) {
        // There are several different possible states depending on the explain verbosity as well as
        // the other stages in the pipeline:
        //  * If verbosity is queryPlanner, then the sub-pipeline should be untouched and we can
        //  explain it directly.
        //  * If verbosity is execStats or allPlansExecution, then whether or not to explain the
        //  sub-pipeline depends on if we've started reading from it. For instance, there could be a
        //  $limit stage after the $unionWith which results in only reading from the base collection
        //  branch and not the sub-pipeline.
        Pipeline* pipeCopy = nullptr;
        if (*explain == ExplainOptions::Verbosity::kQueryPlanner) {
            pipeCopy = Pipeline::create(_pipeline->getSources(), _pipeline->getContext()).release();
        } else if (*explain >= ExplainOptions::Verbosity::kExecStats &&
                   _executionState > ExecutionProgress::kIteratingSource) {
            // We've either exhausted the sub-pipeline or at least started iterating it. Use the
            // cached pipeline to get the explain output since the '_pipeline' may have been
            // modified for any optimizations or pushdowns into the initial $cursor stage.
            pipeCopy = Pipeline::create(_cachedPipeline, _pipeline->getContext()).release();
        } else {
            // The plan does not require reading from the sub-pipeline, so just include the
            // serialization in the explain output.
            BSONArrayBuilder bab;
            for (auto&& stage : _pipeline->serialize())
                bab << stage;
            auto spec = collectionless
                ? DOC("pipeline" << bab.arr())
                : DOC("coll" << _pipeline->getContext()->ns.coll() << "pipeline" << bab.arr());
            return Value(DOC(getSourceName() << spec));
        }

        invariant(pipeCopy);
        BSONObj explainLocal =
            pExpCtx->mongoProcessInterface->preparePipelineAndExplain(pipeCopy, *explain);
        LOGV2_DEBUG(4553501, 3, "$unionWith attached cursor to pipeline for explain");
        // We expect this to be an explanation of a pipeline -- there should only be one field.
        invariant(explainLocal.nFields() == 1);

        auto spec = collectionless ? DOC("pipeline" << explainLocal.firstElement())
                                   : DOC("coll" << _pipeline->getContext()->ns.coll() << "pipeline"
                                                << explainLocal.firstElement());
        return Value(DOC(getSourceName() << spec));
    } else {
        BSONArrayBuilder bab;
        for (auto&& stage : _pipeline->serialize())
            bab << stage;
        auto spec = collectionless
            ? DOC("pipeline" << bab.arr())
            : DOC("coll" << _pipeline->getContext()->ns.coll() << "pipeline" << bab.arr());
        return Value(DOC(getSourceName() << spec));
    }
}