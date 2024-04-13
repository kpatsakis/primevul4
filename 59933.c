entry_guard_add_to_sample(guard_selection_t *gs,
                          const node_t *node)
{
  log_info(LD_GUARD, "Adding %s as to the entry guard sample set.",
           node_describe(node));

  /* make sure that the guard is not already sampled. */
  if (BUG(have_sampled_guard_with_id(gs, (const uint8_t*)node->identity)))
    return NULL; // LCOV_EXCL_LINE

  return entry_guard_add_to_sample_impl(gs,
                                        (const uint8_t*)node->identity,
                                        node_get_nickname(node),
                                        NULL);
}
