dissect_common_dynamic_pusch_assignment(packet_info *pinfo, proto_tree *tree,
                                        tvbuff_t *tvb, int offset)
{
    guint8 pusch_set_id;
    guint8 activation_cfn;
    guint8 duration;

    /* PUSCH Set Id */
    pusch_set_id = tvb_get_guint8(tvb, offset);
    proto_tree_add_item(tree, hf_fp_pusch_set_id, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset++;

    /* Activation CFN */
    activation_cfn = tvb_get_guint8(tvb, offset);
    proto_tree_add_item(tree, hf_fp_activation_cfn, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset++;

    /* Duration */
    duration = tvb_get_guint8(tvb, offset) * 10;
    proto_tree_add_uint(tree, hf_fp_duration, tvb, offset, 1, duration);
    offset++;

    col_append_fstr(pinfo->cinfo, COL_INFO,
                    "   PUSCH Set Id=%u  Activation CFN=%u  Duration=%u",
                    pusch_set_id, activation_cfn, duration);

    return offset;
}
