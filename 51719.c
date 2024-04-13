dissect_rpcap_open_request (tvbuff_t *tvb, packet_info *pinfo _U_,
                            proto_tree *parent_tree, gint offset)
{
  gint len;

  len = tvb_captured_length_remaining (tvb, offset);
  proto_tree_add_item (parent_tree, hf_open_request, tvb, offset, len, ENC_ASCII|ENC_NA);
}
