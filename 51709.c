dissect_rpcap_auth_request (tvbuff_t *tvb, packet_info *pinfo _U_,
                            proto_tree *parent_tree, gint offset)
{
  proto_tree *tree;
  proto_item *ti;
  guint16 type, slen1, slen2;

  ti = proto_tree_add_item (parent_tree, hf_auth_request, tvb, offset, -1, ENC_NA);
  tree = proto_item_add_subtree (ti, ett_auth_request);

  type = tvb_get_ntohs (tvb, offset);
  proto_tree_add_item (tree, hf_auth_type, tvb, offset, 2, ENC_BIG_ENDIAN);
  offset += 2;

  proto_tree_add_item (tree, hf_dummy, tvb, offset, 2, ENC_BIG_ENDIAN);
  offset += 2;

  slen1 = tvb_get_ntohs (tvb, offset);
  proto_tree_add_item (tree, hf_auth_slen1, tvb, offset, 2, ENC_BIG_ENDIAN);
  offset += 2;

  slen2 = tvb_get_ntohs (tvb, offset);
  proto_tree_add_item (tree, hf_auth_slen2, tvb, offset, 2, ENC_BIG_ENDIAN);
  offset += 2;

  if (type == RPCAP_RMTAUTH_NULL) {
    proto_item_append_text (ti, " (none)");
  } else if (type == RPCAP_RMTAUTH_PWD) {
    guint8 *username, *password;

    username = tvb_get_string_enc(wmem_packet_scope(), tvb, offset, slen1, ENC_ASCII);
    proto_tree_add_item (tree, hf_auth_username, tvb, offset, slen1, ENC_ASCII|ENC_NA);
    offset += slen1;

    password = tvb_get_string_enc(wmem_packet_scope(), tvb, offset, slen2, ENC_ASCII);
    proto_tree_add_item (tree, hf_auth_password, tvb, offset, slen2, ENC_ASCII|ENC_NA);
    offset += slen2;

    proto_item_append_text (ti, " (%s/%s)", username, password);
  }
  return offset;
}
