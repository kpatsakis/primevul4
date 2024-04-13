dissect_rpcap_open_reply (tvbuff_t *tvb, packet_info *pinfo _U_,
                          proto_tree *parent_tree, gint offset)
{
  proto_tree *tree;
  proto_item *ti;

  ti = proto_tree_add_item (parent_tree, hf_open_reply, tvb, offset, -1, ENC_NA);
  tree = proto_item_add_subtree (ti, ett_open_reply);

  linktype = tvb_get_ntohl (tvb, offset);
  proto_tree_add_item (tree, hf_linktype, tvb, offset, 4, ENC_BIG_ENDIAN);
  offset += 4;

  proto_tree_add_item (tree, hf_tzoff, tvb, offset, 4, ENC_BIG_ENDIAN);
}
