dissect_dch_ul_synchronisation(proto_tree *tree, packet_info *pinfo, tvbuff_t *tvb, int offset)
{
    guint8 cfn;
    gint16 toa;

    /* CFN control */
    cfn = tvb_get_guint8(tvb, offset);
    proto_tree_add_item(tree, hf_fp_cfn_control, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset++;

    /* ToA */
    toa = tvb_get_ntohs(tvb, offset);
    proto_tree_add_item(tree, hf_fp_toa, tvb, offset, 2, ENC_BIG_ENDIAN);
    offset += 2;

    col_append_fstr(pinfo->cinfo, COL_INFO, " CFN = %u, ToA = %d",
                    cfn, toa);

    return offset;
}
