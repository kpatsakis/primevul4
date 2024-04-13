  UpdateTemporaryGlobalQuotaTask(
      QuotaManager* manager,
      int64 new_quota,
      QuotaCallback* callback)
      : DatabaseTaskBase(manager),
        new_quota_(new_quota),
        callback_(callback) {
    DCHECK_GE(new_quota, 0);
  }
