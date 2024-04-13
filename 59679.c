lock_file_new (const char *path)
{
  LockFile *lock = xcalloc (sizeof (LockFile));

  lock->path = path;
  if (last_lock_file != NULL)
    last_lock_file->next = lock;
  else
    lock_files = lock;

  last_lock_file = lock;
  return lock;
}
