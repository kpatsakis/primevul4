dissect_rpcap_stats_reply (tvbuff_t *tvb, packet_info *pinfo _U_,
                           proto_tree *parent_tree, gint offset)
{
  proto_tree *tree;
  proto_item *ti;

  ti = proto_tree_add_item (parent_tree, hf_stats_reply, tvb, offset, 16, ENC_NA);
  tree = proto_item_add_subtree (ti, ett_stats_reply);

  proto_tree_add_item (tree, hf_ifrecv, tvb, offset, 4, ENC_BIG_ENDIAN);
  offset += 4;

  proto_tree_add_item (tree, hf_ifdrop, tvb, offset, 4, ENC_BIG_ENDIAN);
  offset += 4;

  proto_tree_add_item (tree, hf_krnldrop, tvb, offset, 4, ENC_BIG_ENDIAN);
  offset += 4;

  proto_tree_add_item (tree, hf_srvcapt, tvb, offset, 4, ENC_BIG_ENDIAN);
}
