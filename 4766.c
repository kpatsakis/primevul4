std::unique_ptr<DocumentSourceUnionWith::LiteParsed> DocumentSourceUnionWith::LiteParsed::parse(
    const NamespaceString& nss, const BSONElement& spec) {
    uassert(ErrorCodes::FailedToParse,
            str::stream()
                << "the $unionWith stage specification must be an object or string, but found "
                << typeName(spec.type()),
            spec.type() == BSONType::Object || spec.type() == BSONType::String);

    NamespaceString unionNss;
    boost::optional<LiteParsedPipeline> liteParsedPipeline;
    if (spec.type() == BSONType::String) {
        unionNss = NamespaceString(nss.db(), spec.valueStringData());
    } else {
        auto unionWithSpec =
            UnionWithSpec::parse(IDLParserErrorContext(kStageName), spec.embeddedObject());
        if (unionWithSpec.getColl()) {
            unionNss = NamespaceString(nss.db(), *unionWithSpec.getColl());
        } else {
            // If no collection specified, it must have $documents as first field in pipeline.
            validateUnionWithCollectionlessPipeline(unionWithSpec.getPipeline());
            unionNss = NamespaceString::makeCollectionlessAggregateNSS(nss.db());
        }

        // Recursively lite parse the nested pipeline, if one exists.
        if (unionWithSpec.getPipeline()) {
            liteParsedPipeline = LiteParsedPipeline(unionNss, *unionWithSpec.getPipeline());
        }
    }

    return std::make_unique<DocumentSourceUnionWith::LiteParsed>(
        spec.fieldName(), std::move(unionNss), std::move(liteParsedPipeline));
}