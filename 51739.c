dissect_common_dl_synchronisation(packet_info *pinfo, proto_tree *tree,
                                  tvbuff_t *tvb, int offset, struct fp_info *p_fp_info)
{
    guint16 cfn;

    if (p_fp_info->channel != CHANNEL_PCH) {
        /* CFN control */
        cfn = tvb_get_guint8(tvb, offset);
        proto_tree_add_item(tree, hf_fp_cfn_control, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset++;
    }
    else {
        /* PCH CFN is 12 bits */
        cfn = (tvb_get_ntohs(tvb, offset) >> 4);
        proto_tree_add_item(tree, hf_fp_pch_cfn, tvb, offset, 2, ENC_BIG_ENDIAN);

        /* 4 bits of padding follow... */
        offset += 2;
    }

    col_append_fstr(pinfo->cinfo, COL_INFO, "   CFN=%u", cfn);

    return offset;
}
