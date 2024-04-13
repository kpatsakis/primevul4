RE_NODE* yr_re_node_create(
    int type,
    RE_NODE* left,
    RE_NODE* right)
{
  RE_NODE* result = (RE_NODE*) yr_malloc(sizeof(RE_NODE));

  if (result != NULL)
  {
    result->type = type;
    result->left = left;
    result->right = right;
    result->greedy = TRUE;
    result->forward_code = NULL;
    result->backward_code = NULL;
  }

  return result;
}
