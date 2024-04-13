  string DebugString() const override {
    std::unique_lock<std::mutex> lock(mu_);
    return strings::StrCat("Staging size: ", buf_.size());
  }