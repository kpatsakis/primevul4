int yr_re_ast_split_at_chaining_point(
    RE_AST* re_ast,
    RE_AST** result_re_ast,
    RE_AST** remainder_re_ast,
    int32_t* min_gap,
    int32_t* max_gap)
{
  RE_NODE* node = re_ast->root_node;
  RE_NODE* child = re_ast->root_node->left;
  RE_NODE* parent = NULL;

  int result;

  *result_re_ast = re_ast;
  *remainder_re_ast = NULL;
  *min_gap = 0;
  *max_gap = 0;

  while (child != NULL && child->type == RE_NODE_CONCAT)
  {
    if (child->right != NULL &&
        child->right->type == RE_NODE_RANGE_ANY &&
        child->right->greedy == FALSE &&
        (child->right->start > STRING_CHAINING_THRESHOLD ||
         child->right->end > STRING_CHAINING_THRESHOLD))
    {
      result = yr_re_ast_create(remainder_re_ast);

      if (result != ERROR_SUCCESS)
        return result;

      (*remainder_re_ast)->root_node = child->left;
      (*remainder_re_ast)->flags = re_ast->flags;

      child->left = NULL;

      if (parent != NULL)
        parent->left = node->right;
      else
        (*result_re_ast)->root_node = node->right;

      node->right = NULL;

      *min_gap = child->right->start;
      *max_gap = child->right->end;

      yr_re_node_destroy(node);

      return ERROR_SUCCESS;
    }

    parent = node;
    node = child;
    child = child->left;
  }

  return ERROR_SUCCESS;
}
