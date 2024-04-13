void DocumentSourceUnionWith::detachFromOperationContext() {
    // We have a pipeline we're going to be executing across multiple calls to getNext(), so we
    // use Pipeline::detachFromOperationContext() to take care of updating the Pipeline's
    // ExpressionContext.
    if (_pipeline) {
        _pipeline->detachFromOperationContext();
    }
}