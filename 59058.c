int compiler_set_root(struct block *blk)
{
   BUG_IF(blk == NULL);
   tree_root = blk;
   return E_SUCCESS;
}
