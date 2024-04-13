dissect_rpcap_findalldevs_reply (tvbuff_t *tvb, packet_info *pinfo _U_,
                                 proto_tree *parent_tree, gint offset, guint16 no_devs)
{
  proto_tree *tree;
  proto_item *ti;
  guint16 i;

  ti = proto_tree_add_item (parent_tree, hf_findalldevs_reply, tvb, offset, -1, ENC_NA);
  tree = proto_item_add_subtree (ti, ett_findalldevs_reply);

  for (i = 0; i < no_devs; i++) {
    offset = dissect_rpcap_findalldevs_if (tvb, pinfo, tree, offset);
    if (tvb_reported_length_remaining (tvb, offset) < 0) {
      /* No more data in packet */
      expert_add_info(pinfo, ti, &ei_no_more_data);
      break;
    }
  }

  proto_item_append_text (ti, ", %d item%s", no_devs, plurality (no_devs, "", "s"));
}
