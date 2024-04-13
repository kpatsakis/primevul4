proc_setgroups_write (const long child_pid, const char * const str)
{
  const size_t str_len = strlen(str);

  char setgroups_path[sizeof ("/proc//setgroups") + INT_STRLEN_BOUND (long)];

  snprintf (setgroups_path, sizeof (setgroups_path),
	    "/proc/%ld/setgroups", child_pid);

  const int fd = open (setgroups_path, O_WRONLY);

  if (fd < 0)
    {
      TEST_VERIFY_EXIT (errno == ENOENT);
      FAIL_UNSUPPORTED ("/proc/%ld/setgroups not found\n", child_pid);
    }

  xwrite (fd, str, str_len);
  xclose(fd);
}