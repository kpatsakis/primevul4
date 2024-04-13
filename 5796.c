  bool WouldExceedMemoryLimit(std::size_t bytes) const {
    return bytes + current_bytes_ > memory_limit_;
  }