umts_fp_init_protocol(void)
{
    guint32 hosta_addr[4];
    guint32 hostb_addr[4];
    address     src_addr, dst_addr;
    conversation_t *conversation;
    umts_fp_conversation_info_t *umts_fp_conversation_info;
    guint i, j, num_tf;

    for (i=0; i<num_umts_fp_ep_and_ch_items; i++) {
        /* check if we have a conversation allready */

        /* Convert the strings to ADDR */
        if (uat_umts_fp_ep_and_ch_records[i].protocol == UMTS_FP_IPV4) {
            if ((uat_umts_fp_ep_and_ch_records[i].srcIP) && (!str_to_ip(uat_umts_fp_ep_and_ch_records[i].srcIP, &hosta_addr))) {
                continue; /* parsing failed, skip this entry */
            }
            if ((uat_umts_fp_ep_and_ch_records[i].dstIP) && (!str_to_ip(uat_umts_fp_ep_and_ch_records[i].dstIP, &hostb_addr))) {
                continue; /* parsing failed, skip this entry */
            }
            set_address(&src_addr, AT_IPv4, 4, &hosta_addr);
            set_address(&dst_addr, AT_IPv4, 4, &hostb_addr);
        } else {
            continue; /* Not implemented yet */
        }
        conversation = find_conversation(1, &src_addr, &dst_addr, PT_UDP, uat_umts_fp_ep_and_ch_records[i].src_port, 0, NO_ADDR2|NO_PORT2);
        if (conversation == NULL) {
            /* It's not part of any conversation - create a new one. */
            conversation = conversation_new(1, &src_addr, &dst_addr, PT_UDP, uat_umts_fp_ep_and_ch_records[i].src_port, 0, NO_ADDR2|NO_PORT2);
            if (conversation == NULL)
                continue;
            conversation_set_dissector(conversation, fp_handle);
            switch (uat_umts_fp_ep_and_ch_records[i].channel_type) {
            case CHANNEL_RACH_FDD:
                /* set up conversation info for RACH FDD channels */
                umts_fp_conversation_info = wmem_new0(wmem_file_scope(), umts_fp_conversation_info_t);
                /* Fill in the data */
                umts_fp_conversation_info->iface_type        = (enum fp_interface_type)uat_umts_fp_ep_and_ch_records[i].interface_type;
                umts_fp_conversation_info->division          = (enum division_type) uat_umts_fp_ep_and_ch_records[i].division;
                umts_fp_conversation_info->channel           = uat_umts_fp_ep_and_ch_records[i].channel_type;
                umts_fp_conversation_info->dl_frame_number   = 0;
                umts_fp_conversation_info->ul_frame_number   = 1;
                copy_address_wmem(wmem_file_scope(), &(umts_fp_conversation_info->crnc_address), &src_addr);
                umts_fp_conversation_info->crnc_port         = uat_umts_fp_ep_and_ch_records[i].src_port;
                umts_fp_conversation_info->rlc_mode          = (enum fp_rlc_mode) uat_umts_fp_ep_and_ch_records[i].rlc_mode;
                /*Save unique UE-identifier */
                umts_fp_conversation_info->com_context_id = 1;


                /* DCH's in this flow */
                umts_fp_conversation_info->dch_crc_present = 2;
                /* Set data for First or single channel */
                umts_fp_conversation_info->fp_dch_channel_info[0].num_ul_chans = num_tf = 1;

                for (j = 0; j < num_tf; j++) {
                    umts_fp_conversation_info->fp_dch_channel_info[0].ul_chan_tf_size[j] = 168;
                    umts_fp_conversation_info->fp_dch_channel_info[0].ul_chan_num_tbs[j] = 1;
                }

                umts_fp_conversation_info->dchs_in_flow_list[0] = 1;
                umts_fp_conversation_info->num_dch_in_flow=1;
                set_umts_fp_conv_data(conversation, umts_fp_conversation_info);
            default:
                break;
            }
        }
    }
}
