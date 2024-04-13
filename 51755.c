dissect_dch_tnl_congestion_indication(proto_tree *tree, packet_info *pinfo, tvbuff_t *tvb, int offset)
{
    guint64 status;

    /* Congestion status */
    proto_tree_add_bits_ret_val(tree, hf_fp_congestion_status, tvb,
                                offset*8 + 6, 2, &status, ENC_BIG_ENDIAN);
    offset++;

    col_append_fstr(pinfo->cinfo, COL_INFO, " status = %s",
                    val_to_str_const((guint16)status, congestion_status_vals, "unknown"));

    return offset;
}
