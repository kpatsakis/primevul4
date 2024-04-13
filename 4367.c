  bool isDraining() {
    return drainTimeout_.isScheduled();
  }