  void Clear() {
    std::unique_lock<std::mutex> lock(mu_);
    buf_.clear();
    current_bytes_ = 0;

    notify_inserters_if_bounded(&lock);
  }