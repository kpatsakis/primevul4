history_tree_clear(OnigCaptureTreeNode* node)
{
  int i;

  if (IS_NOT_NULL(node)) {
    for (i = 0; i < node->num_childs; i++) {
      if (IS_NOT_NULL(node->childs[i])) {
        history_tree_free(node->childs[i]);
      }
    }
    for (i = 0; i < node->allocated; i++) {
      node->childs[i] = (OnigCaptureTreeNode* )0;
    }
    node->num_childs = 0;
    node->beg = ONIG_REGION_NOTPOS;
    node->end = ONIG_REGION_NOTPOS;
    node->group = -1;
  }
}
