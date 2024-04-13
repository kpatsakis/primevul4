int uv_process_kill(uv_process_t* process, int signum) {
  return uv_kill(process->pid, signum);
}
