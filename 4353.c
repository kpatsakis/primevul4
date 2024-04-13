do_test (void)
{
  base = support_create_and_chdir_toolong_temp_directory (BASENAME);

  xmkdir (MOUNT_NAME, S_IRWXU);
  atexit (do_cleanup);

  TEST_VERIFY_EXIT (socketpair (AF_UNIX, SOCK_STREAM, 0, sockfd) == 0);
  pid_t child_pid = xclone (child_func, NULL, child_stack,
			    sizeof (child_stack),
			    CLONE_NEWUSER | CLONE_NEWNS | SIGCHLD);

  xclose (sockfd[1]);
  const int sock = sockfd[0];

  char map_path[sizeof ("/proc//uid_map") + INT_STRLEN_BOUND (long)];
  char map_buf[sizeof ("0  1") + INT_STRLEN_BOUND (long)];

  snprintf (map_path, sizeof (map_path), "/proc/%ld/uid_map",
	    (long) child_pid);
  snprintf (map_buf, sizeof (map_buf), "0 %ld 1", (long) getuid());
  update_map (map_buf, map_path);

  proc_setgroups_write ((long) child_pid, "deny");
  snprintf (map_path, sizeof (map_path), "/proc/%ld/gid_map",
	    (long) child_pid);
  snprintf (map_buf, sizeof (map_buf), "0 %ld 1", (long) getgid());
  update_map (map_buf, map_path);

  TEST_VERIFY_EXIT (send (sock, "1", 1, MSG_NOSIGNAL) == 1);
  const int fd = recv_fd (sock);
  TEST_VERIFY_EXIT (fd >= 0);
  TEST_VERIFY_EXIT (fchdir (fd) == 0);

  static char buf[2 * 10 + 1];
  memset (buf, 'A', sizeof (buf));

  /* Finally, call getcwd and check if it resulted in a buffer underflow.  */
  char * cwd = getcwd (buf + sizeof (buf) / 2, 1);
  TEST_VERIFY (cwd == NULL);
  TEST_VERIFY (errno == ERANGE);

  for (int i = 0; i < sizeof (buf); i++)
    if (buf[i] != 'A')
      {
	printf ("buf[%d] = %02x\n", i, (unsigned int) buf[i]);
	support_record_failure ();
      }

  TEST_VERIFY_EXIT (send (sock, "a", 1, MSG_NOSIGNAL) == 1);
  xclose (sock);
  TEST_VERIFY_EXIT (xwaitpid (child_pid, NULL, 0) == child_pid);

  return 0;
}