void QuotaManager::DeleteOriginFromDatabase(
    const GURL& origin, StorageType type) {
  LazyInitialize();
  if (db_disabled_)
    return;
  scoped_refptr<DeleteOriginInfo> task =
      new DeleteOriginInfo(this, origin, type);
  task->Start();
}
