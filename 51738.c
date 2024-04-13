dissect_common_dl_node_synchronisation(packet_info *pinfo, proto_tree *tree,
                                       tvbuff_t *tvb, int offset)
{
    /* T1 (3 bytes) */
    guint32 t1 = tvb_get_ntoh24(tvb, offset);
    proto_tree_add_item(tree, hf_fp_t1, tvb, offset, 3, ENC_BIG_ENDIAN);
    offset += 3;

    col_append_fstr(pinfo->cinfo, COL_INFO, "   T1=%u", t1);

    return offset;
}
