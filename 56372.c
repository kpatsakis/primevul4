static void DestroyCubeInfo(CubeInfo *cube_info)
{
  register Nodes
    *nodes;

  /*
    Release color cube tree storage.
  */
  do
  {
    nodes=cube_info->node_queue->next;
    cube_info->node_queue->nodes=(NodeInfo *) RelinquishMagickMemory(
      cube_info->node_queue->nodes);
    cube_info->node_queue=(Nodes *) RelinquishMagickMemory(
      cube_info->node_queue);
    cube_info->node_queue=nodes;
  } while (cube_info->node_queue != (Nodes *) NULL);
  if (cube_info->memory_info != (MemoryInfo *) NULL)
    cube_info->memory_info=RelinquishVirtualMemory(cube_info->memory_info);
  cube_info->quantize_info=DestroyQuantizeInfo(cube_info->quantize_info);
  cube_info=(CubeInfo *) RelinquishMagickMemory(cube_info);
}
