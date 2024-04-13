static NodeInfo *GetNodeInfo(CubeInfo *cube_info,const size_t id,
  const size_t level,NodeInfo *parent)
{
  NodeInfo
    *node_info;

  if (cube_info->free_nodes == 0)
    {
      Nodes
        *nodes;

      /*
        Allocate a new queue of nodes.
      */
      nodes=(Nodes *) AcquireMagickMemory(sizeof(*nodes));
      if (nodes == (Nodes *) NULL)
        return((NodeInfo *) NULL);
      nodes->nodes=(NodeInfo *) AcquireQuantumMemory(NodesInAList,
        sizeof(*nodes->nodes));
      if (nodes->nodes == (NodeInfo *) NULL)
        return((NodeInfo *) NULL);
      nodes->next=cube_info->node_queue;
      cube_info->node_queue=nodes;
      cube_info->next_node=nodes->nodes;
      cube_info->free_nodes=NodesInAList;
    }
  cube_info->nodes++;
  cube_info->free_nodes--;
  node_info=cube_info->next_node++;
  (void) ResetMagickMemory(node_info,0,sizeof(*node_info));
  node_info->parent=parent;
  node_info->id=id;
  node_info->level=level;
  return(node_info);
}
