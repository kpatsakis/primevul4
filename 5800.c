  void notify_inserters_if_bounded(std::unique_lock<std::mutex>* lock) {
    if (IsBounded()) {
      lock->unlock();
      // Notify all inserters. The removal of an element
      // may make memory available for many inserters
      // to insert new elements
      full_cond_var_.notify_all();
    }
  }