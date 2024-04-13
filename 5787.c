  Status Put(Tuple* tuple) {
    std::unique_lock<std::mutex> lock(mu_);

    std::size_t tuple_bytes = GetTupleBytes(*tuple);

    // Sanity check so that we don't block for ever below
    if (memory_limit_ > 0 && tuple_bytes > memory_limit_) {
      return Status(
          errors::ResourceExhausted("Attempted to insert "
                                    "tensors with combined size of '",
                                    tuple_bytes,
                                    "' bytes into "
                                    "Staging Area with a memory limit of '",
                                    memory_limit_, "'."));
    }

    // If buffer capacity is bounded wait until elements have been removed
    if (IsBounded()) {
      full_cond_var_.wait(lock, [tuple_bytes, this]() {
        // If there's a memory limit, check if there's space for insertion
        bool memory_limit_valid =
            memory_limit_ > 0 ? !WouldExceedMemoryLimit(tuple_bytes) : true;
        // If we're configured for capacity check if there's space for insertion
        bool capacity_valid = capacity_ > 0 ? !IsCapacityFull() : true;

        // Stop waiting upon success for both conditions
        return capacity_valid && memory_limit_valid;
      });
    }

    // Update bytes in the Staging Area
    current_bytes_ += tuple_bytes;

    // Store tuple
    buf_.push_back(std::move(*tuple));

    lock.unlock();
    // Notify all removers. Removers
    // may be peeking at a specific element or waiting
    // for the element at the front of the deque.
    // As we don't know the appropriate one to wake up
    // we should wake them all.
    non_empty_cond_var_.notify_all();

    return Status::OK();
  }