dissect_rpcap_findalldevs_if (tvbuff_t *tvb, packet_info *pinfo _U_,
                              proto_tree *parent_tree, gint offset)
{
  proto_tree *tree;
  proto_item *ti;
  guint16 namelen, desclen, naddr, i;
  gint boffset = offset;

  ti = proto_tree_add_item (parent_tree, hf_findalldevs_if, tvb, offset, -1, ENC_NA);
  tree = proto_item_add_subtree (ti, ett_findalldevs_if);

  namelen = tvb_get_ntohs (tvb, offset);
  proto_tree_add_item (tree, hf_namelen, tvb, offset, 2, ENC_BIG_ENDIAN);
  offset += 2;

  desclen = tvb_get_ntohs (tvb, offset);
  proto_tree_add_item (tree, hf_desclen, tvb, offset, 2, ENC_BIG_ENDIAN);
  offset += 2;

  proto_tree_add_item (tree, hf_if_flags, tvb, offset, 4, ENC_BIG_ENDIAN);
  offset += 4;

  naddr = tvb_get_ntohs (tvb, offset);
  proto_tree_add_item (tree, hf_naddr, tvb, offset, 2, ENC_BIG_ENDIAN);
  offset += 2;

  proto_tree_add_item (tree, hf_dummy, tvb, offset, 2, ENC_BIG_ENDIAN);
  offset += 2;

  if (namelen) {
    proto_item_append_text (ti, ": %s", tvb_get_string_enc(wmem_packet_scope(), tvb, offset, namelen, ENC_ASCII));
    proto_tree_add_item (tree, hf_if_name, tvb, offset, namelen, ENC_ASCII|ENC_NA);
    offset += namelen;
  }

  if (desclen) {
    proto_tree_add_item (tree, hf_if_desc, tvb, offset, desclen, ENC_ASCII|ENC_NA);
    offset += desclen;
  }

  for (i = 0; i < naddr; i++) {
    offset = dissect_rpcap_findalldevs_ifaddr (tvb, pinfo, tree, offset);
    if (tvb_reported_length_remaining (tvb, offset) < 0) {
      /* No more data in packet */
      expert_add_info(pinfo, ti, &ei_no_more_data);
      break;
    }
  }

  proto_item_set_len (ti, offset - boffset);

  return offset;
}
