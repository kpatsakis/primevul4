static void print_guid(ff_asf_guid *g)
{
    int i;
    PRINT_IF_GUID(g, ff_asf_header);
    else PRINT_IF_GUID(g, ff_asf_file_header);
    else PRINT_IF_GUID(g, ff_asf_stream_header);
    else PRINT_IF_GUID(g, ff_asf_audio_stream);
    else PRINT_IF_GUID(g, ff_asf_audio_conceal_none);
    else PRINT_IF_GUID(g, ff_asf_video_stream);
    else PRINT_IF_GUID(g, ff_asf_video_conceal_none);
    else PRINT_IF_GUID(g, ff_asf_command_stream);
    else PRINT_IF_GUID(g, ff_asf_comment_header);
    else PRINT_IF_GUID(g, ff_asf_codec_comment_header);
    else PRINT_IF_GUID(g, ff_asf_codec_comment1_header);
    else PRINT_IF_GUID(g, ff_asf_data_header);
    else PRINT_IF_GUID(g, ff_asf_simple_index_header);
    else PRINT_IF_GUID(g, ff_asf_head1_guid);
    else PRINT_IF_GUID(g, ff_asf_head2_guid);
    else PRINT_IF_GUID(g, ff_asf_my_guid);
    else PRINT_IF_GUID(g, ff_asf_ext_stream_header);
    else PRINT_IF_GUID(g, ff_asf_extended_content_header);
    else PRINT_IF_GUID(g, ff_asf_ext_stream_embed_stream_header);
    else PRINT_IF_GUID(g, ff_asf_ext_stream_audio_stream);
    else PRINT_IF_GUID(g, ff_asf_metadata_header);
    else PRINT_IF_GUID(g, ff_asf_metadata_library_header);
    else PRINT_IF_GUID(g, ff_asf_marker_header);
    else PRINT_IF_GUID(g, stream_bitrate_guid);
    else PRINT_IF_GUID(g, ff_asf_language_guid);
    else
        av_log(NULL, AV_LOG_TRACE, "(GUID: unknown) ");
    for (i = 0; i < 16; i++)
        av_log(NULL, AV_LOG_TRACE, " 0x%02x,", (*g)[i]);
    av_log(NULL, AV_LOG_TRACE, "}\n");
