guard_has_descriptor(const entry_guard_t *guard)
{
  const node_t *node = node_get_by_id(guard->identity);
  if (!node)
    return 0;
  return node_has_descriptor(node);
}
