  AvailableSpaceQueryTask(
      QuotaManager* manager,
      scoped_refptr<base::MessageLoopProxy> db_message_loop,
      const FilePath& profile_path,
      AvailableSpaceCallback* callback)
      : QuotaThreadTask(manager, db_message_loop),
        profile_path_(profile_path),
        space_(-1),
        callback_(callback) {}
