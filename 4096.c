void CollectGraphs(EagerContext* ctx) {
  mutex_lock ml(*ctx->MetadataMu());

  GraphCollector* collector = ctx->GetGraphCollector();
  mutex_lock mll(collector->mu);

  // Adding to partition graphs for backward compatibility.
  for (const auto& graph : collector->partitioned_graphs) {
    *ctx->RunMetadataProto()->add_partition_graphs() = graph;
  }

  if (collector->dirty) {
    auto* function_graphs = ctx->RunMetadataProto()->add_function_graphs();
    *function_graphs->mutable_post_optimization_graph() =
        collector->optimized_graph;
    *function_graphs->mutable_pre_optimization_graph() = collector->raw_graph;
    for (const auto& graph : collector->partitioned_graphs) {
      *function_graphs->add_partition_graphs() = graph;
    }
  }

  collector->ClearGraphs();
}