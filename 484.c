bool QuotaManager::ResetUsageTracker(StorageType type) {
  switch (type) {
    case kStorageTypeTemporary:
      if (temporary_usage_tracker_->IsWorking())
        return false;
      temporary_usage_tracker_.reset(
          new UsageTracker(clients_, kStorageTypeTemporary,
                           special_storage_policy_));
      return true;
    case kStorageTypePersistent:
      if (persistent_usage_tracker_->IsWorking())
        return false;
      persistent_usage_tracker_.reset(
          new UsageTracker(clients_, kStorageTypePersistent,
                           special_storage_policy_));
      return true;
    default:
      NOTREACHED();
  }
  return true;
}
