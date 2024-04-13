dissect_rpcap_startcap_reply (tvbuff_t *tvb, packet_info *pinfo _U_,
                              proto_tree *parent_tree, gint offset)
{
  proto_tree *tree;
  proto_item *ti;

  ti = proto_tree_add_item (parent_tree, hf_startcap_reply, tvb, offset, -1, ENC_NA);
  tree = proto_item_add_subtree (ti, ett_startcap_reply);

  proto_tree_add_item (tree, hf_bufsize, tvb, offset, 4, ENC_BIG_ENDIAN);
  offset += 4;

  proto_tree_add_item (tree, hf_server_port, tvb, offset, 2, ENC_BIG_ENDIAN);
  offset += 2;

  proto_tree_add_item (tree, hf_dummy, tvb, offset, 2, ENC_BIG_ENDIAN);
}
