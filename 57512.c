make_capture_history_tree(OnigCaptureTreeNode* node, OnigStackType** kp,
                          OnigStackType* stk_top, UChar* str, regex_t* reg)
{
  int n, r;
  OnigCaptureTreeNode* child;
  OnigStackType* k = *kp;

  while (k < stk_top) {
    if (k->type == STK_MEM_START) {
      n = k->u.mem.num;
      if (n <= ONIG_MAX_CAPTURE_HISTORY_GROUP &&
          BIT_STATUS_AT(reg->capture_history, n) != 0) {
        child = history_node_new();
        CHECK_NULL_RETURN_MEMERR(child);
        child->group = n;
        child->beg = (int )(k->u.mem.pstr - str);
        r = history_tree_add_child(node, child);
        if (r != 0) return r;
        *kp = (k + 1);
        r = make_capture_history_tree(child, kp, stk_top, str, reg);
        if (r != 0) return r;

        k = *kp;
        child->end = (int )(k->u.mem.pstr - str);
      }
    }
    else if (k->type == STK_MEM_END) {
      if (k->u.mem.num == node->group) {
        node->end = (int )(k->u.mem.pstr - str);
        *kp = k;
        return 0;
      }
    }
    k++;
  }

  return 1; /* 1: root node ending. */
}
