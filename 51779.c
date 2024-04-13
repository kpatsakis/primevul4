set_umts_fp_conv_data(conversation_t *conversation, umts_fp_conversation_info_t *umts_fp_conversation_info)
{

    if (conversation == NULL) {
        return;
     }

    conversation_add_proto_data(conversation, proto_fp, umts_fp_conversation_info);
}
