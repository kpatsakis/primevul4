dissect_rpcap_sampling_request (tvbuff_t *tvb, packet_info *pinfo _U_,
                                proto_tree *parent_tree, gint offset)
{
  proto_tree *tree;
  proto_item *ti;
  guint32 value;
  guint8 method;

  ti = proto_tree_add_item (parent_tree, hf_sampling_request, tvb, offset, -1, ENC_NA);
  tree = proto_item_add_subtree (ti, ett_sampling_request);

  method = tvb_get_guint8 (tvb, offset);
  proto_tree_add_item (tree, hf_sampling_method, tvb, offset, 1, ENC_BIG_ENDIAN);
  offset += 1;

  proto_tree_add_item (tree, hf_sampling_dummy1, tvb, offset, 1, ENC_BIG_ENDIAN);
  offset += 1;

  proto_tree_add_item (tree, hf_sampling_dummy2, tvb, offset, 2, ENC_BIG_ENDIAN);
  offset += 2;

  value = tvb_get_ntohl (tvb, offset);
  proto_tree_add_item (tree, hf_sampling_value, tvb, offset, 4, ENC_BIG_ENDIAN);
  offset += 4;

  switch (method) {
  case RPCAP_SAMP_NOSAMP:
    proto_item_append_text (ti, ": None");
    break;
  case RPCAP_SAMP_1_EVERY_N:
    proto_item_append_text (ti, ": 1 every %d", value);
    break;
  case RPCAP_SAMP_FIRST_AFTER_N_MS:
    proto_item_append_text (ti, ": First after %d ms", value);
    break;
  default:
    break;
  }
  return offset;
}
