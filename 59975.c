get_guard_selection_info(void)
{
  if (!curr_guard_context) {
    create_initial_guard_context();
  }

  return curr_guard_context;
}
