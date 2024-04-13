  void Get(Tuple* tuple) {  // TODO(zhifengc): Support cancellation.
    std::unique_lock<std::mutex> lock(mu_);

    // Wait for data if the buffer is empty
    non_empty_cond_var_.wait(lock, [this]() { return !buf_.empty(); });

    // Move data into the output tuple
    *tuple = std::move(buf_.front());
    buf_.pop_front();

    // Update bytes in the Staging Area
    current_bytes_ -= GetTupleBytes(*tuple);

    notify_inserters_if_bounded(&lock);
  }