static void uv__chld(uv_signal_t* handle, int signum) {
  uv_process_t* process;
  uv_loop_t* loop;
  int exit_status;
  int term_signal;
  unsigned int i;
  int status;
  pid_t pid;
  QUEUE pending;
  QUEUE* h;
  QUEUE* q;

  assert(signum == SIGCHLD);

  QUEUE_INIT(&pending);
  loop = handle->loop;

  for (i = 0; i < ARRAY_SIZE(loop->process_handles); i++) {
    h = loop->process_handles + i;
    q = QUEUE_HEAD(h);

    while (q != h) {
      process = QUEUE_DATA(q, uv_process_t, queue);
      q = QUEUE_NEXT(q);

      do
        pid = waitpid(process->pid, &status, WNOHANG);
      while (pid == -1 && errno == EINTR);

      if (pid == 0)
        continue;

      if (pid == -1) {
        if (errno != ECHILD)
          abort();
        continue;
      }

      process->status = status;
      QUEUE_REMOVE(&process->queue);
      QUEUE_INSERT_TAIL(&pending, &process->queue);
    }

    while (!QUEUE_EMPTY(&pending)) {
      q = QUEUE_HEAD(&pending);
      QUEUE_REMOVE(q);
      QUEUE_INIT(q);

      process = QUEUE_DATA(q, uv_process_t, queue);
      uv__handle_stop(process);

      if (process->exit_cb == NULL)
        continue;

      exit_status = 0;
      if (WIFEXITED(process->status))
        exit_status = WEXITSTATUS(process->status);

      term_signal = 0;
      if (WIFSIGNALED(process->status))
        term_signal = WTERMSIG(process->status);

      process->exit_cb(process, exit_status, term_signal);
    }
  }
}
