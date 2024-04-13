static void Reduce(CubeInfo *cube_info,const NodeInfo *node_info)
{
  register ssize_t
    i;

  size_t
    number_children;

  /*
    Traverse any children.
  */
  number_children=cube_info->associate_alpha == MagickFalse ? 8UL : 16UL;
  for (i=0; i < (ssize_t) number_children; i++)
    if (node_info->child[i] != (NodeInfo *) NULL)
      Reduce(cube_info,node_info->child[i]);
  if (node_info->quantize_error <= cube_info->pruning_threshold)
    PruneChild(cube_info,node_info);
  else
    {
      /*
        Find minimum pruning threshold.
      */
      if (node_info->number_unique > 0)
        cube_info->colors++;
      if (node_info->quantize_error < cube_info->next_threshold)
        cube_info->next_threshold=node_info->quantize_error;
    }
}
