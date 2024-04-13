static void PruneToCubeDepth(CubeInfo *cube_info,const NodeInfo *node_info)
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
      PruneToCubeDepth(cube_info,node_info->child[i]);
  if (node_info->level > cube_info->depth)
    PruneChild(cube_info,node_info);
}
