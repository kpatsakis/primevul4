static QUEUE* uv__process_queue(uv_loop_t* loop, int pid) {
  assert(pid > 0);
  return loop->process_handles + pid % ARRAY_SIZE(loop->process_handles);
}
