dissect_rpcap_findalldevs_ifaddr (tvbuff_t *tvb, packet_info *pinfo _U_,
                                  proto_tree *parent_tree, gint offset)
{
  proto_tree *tree;
  proto_item *ti;
  gint boffset = offset;

  ti = proto_tree_add_item (parent_tree, hf_findalldevs_ifaddr, tvb, offset, -1, ENC_NA);
  tree = proto_item_add_subtree (ti, ett_findalldevs_ifaddr);

  offset = dissect_rpcap_ifaddr (tvb, pinfo, tree, offset, hf_if_addr, ti);
  offset = dissect_rpcap_ifaddr (tvb, pinfo, tree, offset, hf_if_netmask, NULL);
  offset = dissect_rpcap_ifaddr (tvb, pinfo, tree, offset, hf_if_broadaddr, NULL);
  offset = dissect_rpcap_ifaddr (tvb, pinfo, tree, offset, hf_if_dstaddr, NULL);

  proto_item_set_len (ti, offset - boffset);

  return offset;
}
