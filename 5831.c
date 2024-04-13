dissect_sysdig_event(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
        void *data _U_)
{
    proto_item *ti;
    proto_tree *se_tree, *syscall_tree;
    guint       event_type = pinfo->rec->rec_header.syscall_header.event_type;
    int         encoding = pinfo->rec->rec_header.syscall_header.byte_order == G_BIG_ENDIAN ? ENC_BIG_ENDIAN : ENC_LITTLE_ENDIAN;
    const struct _event_col_info *cur_col_info;
    const struct _event_tree_info *cur_tree_info;

    /*** HEURISTICS ***/

    /* Check that the packet is long enough for it to belong to us. */
    if (tvb_reported_length(tvb) < SYSDIG_EVENT_MIN_LENGTH)
        return 0;

    /*** COLUMN DATA ***/

    /*
     * Sysdig uses the term "event" internally. So far every event has been
     * a syscall.
     */
    col_set_str(pinfo->cinfo, COL_PROTOCOL, "System Call");

    col_clear(pinfo->cinfo, COL_INFO);
    col_add_str(pinfo->cinfo, COL_INFO, val_to_str(event_type, event_type_vals, "Unknown syscall %u"));
    /*
     * XXX We can ditch this in favor of a simple index when event_col_info
     * is contiguous and in the correct order.
     */
    for (cur_col_info = event_col_info; cur_col_info->params; cur_col_info++) {
        if (cur_col_info->event_type == event_type) {
            const struct _event_col_info_param *cur_param = cur_col_info->params;
            int param_offset = cur_col_info->num_len_fields * 2;

            /* Find the data offset */
            int cur_len_field;
            for (cur_len_field = 0;
                 cur_len_field < cur_col_info->num_len_fields && cur_param->param_name;
                 cur_len_field++) {
                unsigned param_len = tvb_get_guint16(tvb, cur_len_field * 2, encoding);
                if (cur_param->param_num == cur_len_field) {
                    col_append_fstr(pinfo->cinfo, COL_INFO, ", %s=", cur_param->param_name);
                    switch (cur_param->param_ftype) {
                    case FT_STRING:
                        col_append_str(pinfo->cinfo, COL_INFO, format_param_str(tvb, param_offset, param_len));
                        break;
                    case FT_UINT64:
                        col_append_fstr(pinfo->cinfo, COL_INFO, "%" G_GUINT64_FORMAT, tvb_get_guint64(tvb, param_offset, encoding));
                    default:
                        break;
                    }
                    cur_param++;
                }
                param_offset += param_len;
            }
        }
    }

    /*** PROTOCOL TREE ***/

    /* create display subtree for the protocol */
    ti = proto_tree_add_item(tree, proto_sysdig_event, tvb, 0, -1, ENC_NA);

    se_tree = proto_item_add_subtree(ti, ett_sysdig_event);

    proto_tree_add_uint(se_tree, hf_se_cpu_id, tvb, 0, 0, pinfo->rec->rec_header.syscall_header.cpu_id);
    proto_tree_add_uint64(se_tree, hf_se_thread_id, tvb, 0, 0, pinfo->rec->rec_header.syscall_header.thread_id);
    proto_tree_add_uint(se_tree, hf_se_event_length, tvb, 0, 0, pinfo->rec->rec_header.syscall_header.event_len);
    if (pinfo->rec->rec_header.syscall_header.nparams != 0) {
        proto_tree_add_uint(se_tree, hf_se_nparams, tvb, 0, 0, pinfo->rec->rec_header.syscall_header.nparams);
    }
    ti = proto_tree_add_uint(se_tree, hf_se_event_type, tvb, 0, 0, event_type);

    syscall_tree = proto_item_add_subtree(ti, ett_sysdig_syscall);

    for (cur_tree_info = event_tree_info; cur_tree_info->hf_indexes; cur_tree_info++) {
        if (cur_tree_info->event_type == event_type) {
            dissect_event_params(tvb, &pinfo->rec->rec_header.syscall_header, 0, syscall_tree, encoding, cur_tree_info->hf_indexes);
            break;
        }
    }

    /* XXX */
    /* return offset; */
    return pinfo->rec->rec_header.syscall_header.event_len;
}