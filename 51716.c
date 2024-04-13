dissect_rpcap_heur_udp (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data)
{
  if (check_rpcap_heur (tvb, FALSE)) {
    /* This is probably a rpcap udp package */
    dissect_rpcap (tvb, pinfo, tree, data);

    return TRUE;
  }

  return FALSE;
}
