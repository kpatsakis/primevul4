  explicit DatabaseTaskBase(QuotaManager* manager)
      : QuotaThreadTask(manager, manager->db_thread_),
        manager_(manager),
        database_(manager->database_.get()),
        db_disabled_(false) {
    DCHECK(manager_);
    DCHECK(database_);
  }
