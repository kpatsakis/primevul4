dissect_rpcap_ifaddr (tvbuff_t *tvb, packet_info *pinfo,
                      proto_tree *parent_tree, gint offset, int hf_id,
                      proto_item *parent_item)
{
  proto_tree *tree;
  proto_item *ti;
  gchar ipaddr[MAX_ADDR_STR_LEN];
  guint32 ipv4;
  guint16 af;

  ti = proto_tree_add_item (parent_tree, hf_id, tvb, offset, 128, ENC_BIG_ENDIAN);
  tree = proto_item_add_subtree (ti, ett_ifaddr);

  af = tvb_get_ntohs (tvb, offset);
  proto_tree_add_item (tree, hf_if_af, tvb, offset, 2, ENC_BIG_ENDIAN);
  offset += 2;

  if (af == COMMON_AF_INET) {
    proto_tree_add_item (tree, hf_if_port, tvb, offset, 2, ENC_BIG_ENDIAN);
    offset += 2;

    ipv4 = tvb_get_ipv4 (tvb, offset);
    ip_to_str_buf((guint8 *)&ipv4, ipaddr, MAX_ADDR_STR_LEN);
    proto_item_append_text (ti, ": %s", ipaddr);
    if (parent_item) {
      proto_item_append_text (parent_item, ": %s", ipaddr);
    }
    proto_tree_add_item (tree, hf_if_ip, tvb, offset, 4, ENC_BIG_ENDIAN);
    offset += 4;

    proto_tree_add_item (tree, hf_if_padding, tvb, offset, 120, ENC_NA);
    offset += 120;
  } else {
    ti = proto_tree_add_item (tree, hf_if_unknown, tvb, offset, 126, ENC_NA);
    if (af != COMMON_AF_UNSPEC) {
      expert_add_info_format(pinfo, ti, &ei_if_unknown,
                             "Unknown address family: %d", af);
    }
    offset += 126;
  }

  return offset;
}
