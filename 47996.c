void uv__process_close(uv_process_t* handle) {
  /* TODO stop signal watcher when this is the last handle */
  QUEUE_REMOVE(&handle->queue);
  uv__handle_stop(handle);
}
