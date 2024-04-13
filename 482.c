void QuotaManager::DidOriginDataEvicted(
    QuotaStatusCode status) {
  DCHECK(io_thread_->BelongsToCurrentThread());

  if (status != kQuotaStatusOk)
    origins_in_error_[eviction_context_.evicted_origin]++;

  eviction_context_.evict_origin_data_callback->Run(status);
  eviction_context_.evict_origin_data_callback.reset();
}
