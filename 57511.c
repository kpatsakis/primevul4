history_tree_free(OnigCaptureTreeNode* node)
{
  history_tree_clear(node);
  xfree(node);
}
