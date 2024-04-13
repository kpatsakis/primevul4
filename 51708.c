dissect_rpcap (tvbuff_t *tvb, packet_info *pinfo, proto_tree *top_tree, void* data _U_)
{
  proto_tree *tree;
  proto_item *ti;
  tvbuff_t *new_tvb;
  gint len, offset = 0;
  guint8 msg_type;
  guint16 msg_value;

  col_set_str (pinfo->cinfo, COL_PROTOCOL, PSNAME);

  col_clear(pinfo->cinfo, COL_INFO);

  ti = proto_tree_add_item (top_tree, proto_rpcap, tvb, offset, -1, ENC_NA);
  tree = proto_item_add_subtree (ti, ett_rpcap);

  proto_tree_add_item (tree, hf_version, tvb, offset, 1, ENC_BIG_ENDIAN);
  offset++;

  msg_type = tvb_get_guint8 (tvb, offset);
  proto_tree_add_item (tree, hf_type, tvb, offset, 1, ENC_BIG_ENDIAN);
  offset++;

  col_append_fstr (pinfo->cinfo, COL_INFO, "%s",
                     val_to_str (msg_type, message_type, "Unknown: %d"));

  proto_item_append_text (ti, ", %s", val_to_str (msg_type, message_type, "Unknown: %d"));

  msg_value = tvb_get_ntohs (tvb, offset);
  if (msg_type == RPCAP_MSG_ERROR) {
    proto_tree_add_item (tree, hf_error_value, tvb, offset, 2, ENC_BIG_ENDIAN);
  } else {
    proto_tree_add_item (tree, hf_value, tvb, offset, 2, ENC_BIG_ENDIAN);
  }
  offset += 2;

  proto_tree_add_item (tree, hf_plen, tvb, offset, 4, ENC_BIG_ENDIAN);
  offset += 4;


  switch (msg_type) {
  case RPCAP_MSG_ERROR:
    dissect_rpcap_error (tvb, pinfo, tree, offset);
    break;
  case RPCAP_MSG_OPEN_REQ:
    dissect_rpcap_open_request (tvb, pinfo, tree, offset);
    break;
  case RPCAP_MSG_STARTCAP_REQ:
    dissect_rpcap_startcap_request (tvb, pinfo, tree, offset);
    break;
  case RPCAP_MSG_UPDATEFILTER_REQ:
    dissect_rpcap_filter (tvb, pinfo, tree, offset);
    break;
  case RPCAP_MSG_PACKET:
    proto_item_set_len (ti, 28);
    dissect_rpcap_packet (tvb, pinfo, top_tree, tree, offset, ti);
    break;
  case RPCAP_MSG_AUTH_REQ:
    dissect_rpcap_auth_request (tvb, pinfo, tree, offset);
    break;
  case RPCAP_MSG_SETSAMPLING_REQ:
    dissect_rpcap_sampling_request (tvb, pinfo, tree, offset);
    break;
  case RPCAP_MSG_FINDALLIF_REPLY:
    dissect_rpcap_findalldevs_reply (tvb, pinfo, tree, offset, msg_value);
    break;
  case RPCAP_MSG_OPEN_REPLY:
    dissect_rpcap_open_reply (tvb, pinfo, tree, offset);
    break;
  case RPCAP_MSG_STARTCAP_REPLY:
    dissect_rpcap_startcap_reply (tvb, pinfo, tree, offset);
    break;
  case RPCAP_MSG_STATS_REPLY:
    dissect_rpcap_stats_reply (tvb, pinfo, tree, offset);
    break;
  default:
    len = tvb_reported_length_remaining (tvb, offset);
    if (len) {
      /* Yet unknown, dump as data */
      proto_item_set_len (ti, 8);
      new_tvb = tvb_new_subset_remaining (tvb, offset);
      call_dissector (data_handle, new_tvb, pinfo, top_tree);
    }
    break;
  }

  return tvb_captured_length(tvb);
}
