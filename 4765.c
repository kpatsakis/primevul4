DepsTracker::State DocumentSourceUnionWith::getDependencies(DepsTracker* deps) const {
    // Since the $unionWith stage is a simple passthrough, we *could* report SEE_NEXT here in an
    // attempt to get a covered plan for the base collection. The ideal solution would involve
    // pushing down any dependencies to the inner pipeline as well.
    return DepsTracker::State::NOT_SUPPORTED;
}