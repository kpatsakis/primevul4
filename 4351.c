child_func (void * const arg)
{
  xclose (sockfd[0]);
  const int sock = sockfd[1];
  char ch;

  TEST_VERIFY_EXIT (read (sock, &ch, 1) == 1);
  TEST_VERIFY_EXIT (ch == '1');

  if (mount ("/", MOUNT_NAME, NULL, MS_BIND | MS_REC, NULL))
    FAIL_EXIT1 ("mount failed: %m\n");
  const int fd = xopen ("mpoint",
			O_RDONLY | O_PATH | O_DIRECTORY | O_NOFOLLOW, 0);

  send_fd (sock, fd);
  xclose (fd);

  TEST_VERIFY_EXIT (read (sock, &ch, 1) == 1);
  TEST_VERIFY_EXIT (ch == 'a');

  xclose (sock);
  return 0;
}