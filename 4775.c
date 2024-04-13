MONGO_COMPILER_NOINLINE void DocumentSourceUnionWith::logStartingSubPipeline(
    const std::vector<BSONObj>& serializedPipe) {
    LOGV2_DEBUG(23869,
                1,
                "$unionWith attaching cursor to pipeline {pipeline}",
                "pipeline"_attr = serializedPipe);
}