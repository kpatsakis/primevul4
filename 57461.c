void yr_re_node_destroy(
    RE_NODE* node)
{
  if (node->left != NULL)
    yr_re_node_destroy(node->left);

  if (node->right != NULL)
    yr_re_node_destroy(node->right);

  if (node->type == RE_NODE_CLASS)
    yr_free(node->class_vector);

  yr_free(node);
}
