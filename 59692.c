write_uid_gid_map (uid_t sandbox_uid,
                   uid_t parent_uid,
                   uid_t sandbox_gid,
                   uid_t parent_gid,
                   pid_t pid,
                   bool  deny_groups,
                   bool  map_root)
{
  cleanup_free char *uid_map = NULL;
  cleanup_free char *gid_map = NULL;
  cleanup_free char *dir = NULL;
  cleanup_fd int dir_fd = -1;
  uid_t old_fsuid = -1;

  if (pid == -1)
    dir = xstrdup ("self");
  else
    dir = xasprintf ("%d", pid);

  dir_fd = openat (proc_fd, dir, O_RDONLY | O_PATH);
  if (dir_fd < 0)
    die_with_error ("open /proc/%s failed", dir);

  if (map_root && parent_uid != 0 && sandbox_uid != 0)
    uid_map = xasprintf ("0 %d 1\n"
                         "%d %d 1\n", overflow_uid, sandbox_uid, parent_uid);
  else
    uid_map = xasprintf ("%d %d 1\n", sandbox_uid, parent_uid);

  if (map_root && parent_gid != 0 && sandbox_gid != 0)
    gid_map = xasprintf ("0 %d 1\n"
                         "%d %d 1\n", overflow_gid, sandbox_gid, parent_gid);
  else
    gid_map = xasprintf ("%d %d 1\n", sandbox_gid, parent_gid);

  /* We have to be root to be allowed to write to the uid map
   * for setuid apps, so temporary set fsuid to 0 */
  if (is_privileged)
    old_fsuid = setfsuid (0);

  if (write_file_at (dir_fd, "uid_map", uid_map) != 0)
    die_with_error ("setting up uid map");

  if (deny_groups &&
      write_file_at (dir_fd, "setgroups", "deny\n") != 0)
    {
      /* If /proc/[pid]/setgroups does not exist, assume we are
       * running a linux kernel < 3.19, i.e. we live with the
       * vulnerability known as CVE-2014-8989 in older kernels
       * where setgroups does not exist.
       */
      if (errno != ENOENT)
        die_with_error ("error writing to setgroups");
    }

  if (write_file_at (dir_fd, "gid_map", gid_map) != 0)
    die_with_error ("setting up gid map");

  if (is_privileged)
    {
      setfsuid (old_fsuid);
      if (setfsuid (-1) != real_uid)
        die ("Unable to re-set fsuid");
    }
}
