dissect_rpcap_error (tvbuff_t *tvb, packet_info *pinfo,
                     proto_tree *parent_tree, gint offset)
{
  proto_item *ti;
  gint len;

  len = tvb_captured_length_remaining (tvb, offset);
  if (len <= 0)
    return;

  col_append_fstr (pinfo->cinfo, COL_INFO, ": %s",
                   tvb_format_text_wsp (tvb, offset, len));

  ti = proto_tree_add_item (parent_tree, hf_error, tvb, offset, len, ENC_ASCII|ENC_NA);
  expert_add_info_format(pinfo, ti, &ei_error,
                         "Error: %s", tvb_format_text_wsp (tvb, offset, len));
}
