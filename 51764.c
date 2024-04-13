dissect_hsdpa_capacity_request(packet_info *pinfo, proto_tree *tree,
                               tvbuff_t *tvb, int offset)
{
    guint8  priority;
    guint16 user_buffer_size;

    /* CmCH-PI */
    priority = (tvb_get_guint8(tvb, offset) & 0x0f);
    proto_tree_add_item(tree, hf_fp_cmch_pi, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset++;

    /* User buffer size */
    user_buffer_size = tvb_get_ntohs(tvb, offset);
    proto_tree_add_item(tree, hf_fp_user_buffer_size, tvb, offset, 2, ENC_BIG_ENDIAN);
    offset += 2;

    col_append_fstr(pinfo->cinfo, COL_INFO, "      CmCH-PI=%u  User-Buffer-Size=%u",
                    priority, user_buffer_size);

    return offset;
}
