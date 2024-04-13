dissect_rpcap_filter (tvbuff_t *tvb, packet_info *pinfo,
                      proto_tree *parent_tree, gint offset)
{
  proto_tree *tree;
  proto_item *ti;
  guint32 nitems, i;

  ti = proto_tree_add_item (parent_tree, hf_filter, tvb, offset, -1, ENC_NA);
  tree = proto_item_add_subtree (ti, ett_filter);

  proto_tree_add_item (tree, hf_filtertype, tvb, offset, 2, ENC_BIG_ENDIAN);
  offset += 2;

  proto_tree_add_item (tree, hf_dummy, tvb, offset, 2, ENC_BIG_ENDIAN);
  offset += 2;

  nitems = tvb_get_ntohl (tvb, offset);
  proto_tree_add_item (tree, hf_nitems, tvb, offset, 4, ENC_BIG_ENDIAN);
  offset += 4;

  for (i = 0; i < nitems; i++) {
    offset = dissect_rpcap_filterbpf_insn (tvb, pinfo, tree, offset);
    if (tvb_reported_length_remaining (tvb, offset) < 0) {
      /* No more data in packet */
      expert_add_info(pinfo, ti, &ei_no_more_data);
      break;
    }
  }
}
