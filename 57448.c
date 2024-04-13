int _yr_re_node_contains_dot_star(
    RE_NODE* re_node)
{
  if (re_node->type == RE_NODE_STAR && re_node->left->type == RE_NODE_ANY)
    return TRUE;

  if (re_node->left != NULL && _yr_re_node_contains_dot_star(re_node->left))
    return TRUE;

  if (re_node->right != NULL && _yr_re_node_contains_dot_star(re_node->right))
    return TRUE;

  return FALSE;
}
