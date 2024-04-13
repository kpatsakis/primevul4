int uv_kill(int pid, int signum) {
  if (kill(pid, signum))
    return -errno;
  else
    return 0;
}
