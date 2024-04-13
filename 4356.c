send_fd (const int sock, const int fd)
{
  struct msghdr msg = {0};
  union
    {
      struct cmsghdr hdr;
      char buf[CMSG_SPACE (sizeof (int))];
    } cmsgbuf = {0};
  struct cmsghdr *cmsg;
  struct iovec vec;
  char ch = 'A';
  ssize_t n;

  msg.msg_control = &cmsgbuf.buf;
  msg.msg_controllen = sizeof (cmsgbuf.buf);

  cmsg = CMSG_FIRSTHDR (&msg);
  cmsg->cmsg_len = CMSG_LEN (sizeof (int));
  cmsg->cmsg_level = SOL_SOCKET;
  cmsg->cmsg_type = SCM_RIGHTS;
  memcpy (CMSG_DATA (cmsg), &fd, sizeof (fd));

  vec.iov_base = &ch;
  vec.iov_len = 1;
  msg.msg_iov = &vec;
  msg.msg_iovlen = 1;

  while ((n = sendmsg (sock, &msg, 0)) == -1 && errno == EINTR);

  TEST_VERIFY_EXIT (n == 1);
}