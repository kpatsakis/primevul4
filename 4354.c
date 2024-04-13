recv_fd (const int sock)
{
  struct msghdr msg = {0};
  union
    {
      struct cmsghdr hdr;
      char buf[CMSG_SPACE(sizeof(int))];
    } cmsgbuf = {0};
  struct cmsghdr *cmsg;
  struct iovec vec;
  ssize_t n;
  char ch = '\0';
  int fd = -1;

  vec.iov_base = &ch;
  vec.iov_len = 1;
  msg.msg_iov = &vec;
  msg.msg_iovlen = 1;

  msg.msg_control = &cmsgbuf.buf;
  msg.msg_controllen = sizeof (cmsgbuf.buf);

  while ((n = recvmsg (sock, &msg, 0)) == -1 && errno == EINTR);
  if (n != 1 || ch != 'A')
    return -1;

  cmsg = CMSG_FIRSTHDR (&msg);
  if (cmsg == NULL)
    return -1;
  if (cmsg->cmsg_type != SCM_RIGHTS)
    return -1;
  memcpy (&fd, CMSG_DATA (cmsg), sizeof (fd));
  if (fd < 0)
    return -1;
  return fd;
}