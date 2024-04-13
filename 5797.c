  size_t Size() {
    std::unique_lock<std::mutex> lock(mu_);
    return buf_.size();
  }