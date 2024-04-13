Pipeline::SourceContainer::iterator DocumentSourceUnionWith::doOptimizeAt(
    Pipeline::SourceContainer::iterator itr, Pipeline::SourceContainer* container) {
    auto duplicateAcrossUnion = [&](auto&& nextStage) {
        _pipeline->addFinalSource(nextStage->clone());
        auto newStageItr = container->insert(itr, std::move(nextStage));
        container->erase(std::next(itr));
        return newStageItr == container->begin() ? newStageItr : std::prev(newStageItr);
    };
    if (std::next(itr) != container->end()) {
        if (auto nextMatch = dynamic_cast<DocumentSourceMatch*>((*std::next(itr)).get()))
            return duplicateAcrossUnion(nextMatch);
        else if (auto nextProject = dynamic_cast<DocumentSourceSingleDocumentTransformation*>(
                     (*std::next(itr)).get()))
            return duplicateAcrossUnion(nextProject);
    }
    return std::next(itr);
};