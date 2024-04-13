void QuotaManager::GetPersistentHostQuota(const std::string& host,
                                          HostQuotaCallback* callback_ptr) {
  scoped_ptr<HostQuotaCallback> callback(callback_ptr);
  LazyInitialize();
  if (host.empty()) {
    callback->Run(kQuotaStatusOk, host, kStorageTypePersistent, 0);
    return;
  }
  scoped_refptr<GetPersistentHostQuotaTask> task(
      new GetPersistentHostQuotaTask(this, host, callback.release()));
  task->Start();
}
