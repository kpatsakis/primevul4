MONGO_COMPILER_NOINLINE void DocumentSourceUnionWith::logShardedViewFound(
    const ExceptionFor<ErrorCodes::CommandOnShardedViewNotSupportedOnMongod>& e) {
    LOGV2_DEBUG(4556300,
                3,
                "$unionWith found view definition. ns: {ns}, pipeline: {pipeline}. New "
                "$unionWith sub-pipeline: {new_pipe}",
                "ns"_attr = e->getNamespace(),
                "pipeline"_attr = Value(e->getPipeline()),
                "new_pipe"_attr = _pipeline->serializeToBson());
}