  void DumpQuotaTable() {
    quota_entries_.clear();
    quota_manager_->DumpQuotaTable(
        callback_factory_.NewCallback(
            &QuotaManagerTest::DidDumpQuotaTable));
  }
