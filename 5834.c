dissect_event_params(tvbuff_t *tvb, wtap_syscall_header* syscall_header, int offset, proto_tree *tree, int encoding, int * const *hf_indexes)
{
    int len_offset = offset;
    int param_offset;
    int len_size;
    guint32 cur_param;

    switch (syscall_header->record_type) {
        case BLOCK_TYPE_SYSDIG_EVENT_V2_LARGE:
            param_offset = offset + dissect_header_lens_v2_large(tvb, syscall_header, offset, tree, encoding);
            len_size = SYSDIG_PARAM_SIZE_V2_LARGE;
            break;
        case BLOCK_TYPE_SYSDIG_EVENT_V2:
            param_offset = offset + dissect_header_lens_v2(tvb, syscall_header, offset, tree, encoding);
            len_size = SYSDIG_PARAM_SIZE_V2;
            break;
        default:
            param_offset = offset + dissect_header_lens_v1(tvb, offset, tree, encoding, hf_indexes);
            len_size = SYSDIG_PARAM_SIZE;
            break;
    }

    for (cur_param = 0; cur_param < syscall_header->nparams; cur_param++) {
        if (!hf_indexes[cur_param]) {
            // This happens when new params are added to existent events in sysdig,
            // if the event is already mapped in wireshark with a lower number of params.
            // hf_indexes array size would be < than event being dissected, leading to SIGSEGV.
            break;
        }

        guint32 param_len;
        if (syscall_header->record_type == BLOCK_TYPE_SYSDIG_EVENT_V2_LARGE) {
            param_len = tvb_get_guint32(tvb, len_offset, encoding);
        } else {
            param_len = tvb_get_guint16(tvb, len_offset, encoding);
        }
        const int hf_index = *hf_indexes[cur_param];
        if (proto_registrar_get_ftype(hf_index) == FT_STRING) {
            proto_tree_add_string(tree, hf_index, tvb, param_offset, param_len,
                                  format_param_str(tvb, param_offset, param_len));
        } else {
            proto_tree_add_item(tree, hf_index, tvb, param_offset, param_len, encoding);
        }

        param_offset += param_len;
        len_offset += len_size;
    }
    return param_offset - offset;
}