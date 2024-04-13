void validateUnionWithCollectionlessPipeline(
    const boost::optional<std::vector<mongo::BSONObj>>& pipeline) {
    uassert(ErrorCodes::FailedToParse,
            "$unionWith stage without explicit collection must have a pipeline with $documents as "
            "first stage",
            pipeline && pipeline->size() > 0 &&
                !(*pipeline)[0].getField(DocumentSourceDocuments::kStageName).eoo());
}