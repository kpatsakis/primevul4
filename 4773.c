std::unique_ptr<Pipeline, PipelineDeleter> buildPipelineFromViewDefinition(
    const boost::intrusive_ptr<ExpressionContext>& expCtx,
    ExpressionContext::ResolvedNamespace resolvedNs,
    std::vector<BSONObj> currentPipeline) {

    auto validatorCallback = [](const Pipeline& pipeline) {
        const auto& sources = pipeline.getSources();
        std::for_each(sources.begin(), sources.end(), [](auto& src) {
            uassert(31441,
                    str::stream() << src->getSourceName()
                                  << " is not allowed within a $unionWith's sub-pipeline",
                    src->constraints().isAllowedInUnionPipeline());
        });
    };

    // Copy the ExpressionContext of the base aggregation, using the inner namespace instead.
    auto unionExpCtx = expCtx->copyForSubPipeline(resolvedNs.ns);

    if (resolvedNs.pipeline.empty()) {
        return Pipeline::parse(currentPipeline, unionExpCtx, validatorCallback);
    }
    auto resolvedPipeline = std::move(resolvedNs.pipeline);
    resolvedPipeline.reserve(currentPipeline.size() + resolvedPipeline.size());
    resolvedPipeline.insert(resolvedPipeline.end(),
                            std::make_move_iterator(currentPipeline.begin()),
                            std::make_move_iterator(currentPipeline.end()));

    MakePipelineOptions opts;
    opts.attachCursorSource = false;
    opts.validator = validatorCallback;
    return Pipeline::makePipeline(std::move(resolvedPipeline), unionExpCtx, opts);
}