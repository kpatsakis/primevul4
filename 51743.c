dissect_common_ul_node_synchronisation(packet_info *pinfo, proto_tree *tree,
                                       tvbuff_t *tvb, int offset)
{
    guint32 t1, t2, t3;

    /* T1 (3 bytes) */
    t1 = tvb_get_ntoh24(tvb, offset);
    proto_tree_add_item(tree, hf_fp_t1, tvb, offset, 3, ENC_BIG_ENDIAN);
    offset += 3;

    /* T2 (3 bytes) */
    t2 = tvb_get_ntoh24(tvb, offset);
    proto_tree_add_item(tree, hf_fp_t2, tvb, offset, 3, ENC_BIG_ENDIAN);
    offset += 3;

    /* T3 (3 bytes) */
    t3 = tvb_get_ntoh24(tvb, offset);
    proto_tree_add_item(tree, hf_fp_t3, tvb, offset, 3, ENC_BIG_ENDIAN);
    offset += 3;

    col_append_fstr(pinfo->cinfo, COL_INFO, "   T1=%u T2=%u, T3=%u",
                    t1, t2, t3);

    return offset;
}
