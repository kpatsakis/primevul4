gst_asf_demux_reset (GstASFDemux * demux, gboolean chain_reset)
{
  GST_LOG_OBJECT (demux, "resetting");

  gst_segment_init (&demux->segment, GST_FORMAT_UNDEFINED);
  demux->segment_running = FALSE;
  if (demux->adapter && !chain_reset) {
    gst_adapter_clear (demux->adapter);
    g_object_unref (demux->adapter);
    demux->adapter = NULL;
  }
  if (demux->taglist) {
    gst_tag_list_unref (demux->taglist);
    demux->taglist = NULL;
  }
  if (demux->metadata) {
    gst_caps_unref (demux->metadata);
    demux->metadata = NULL;
  }
  if (demux->global_metadata) {
    gst_structure_free (demux->global_metadata);
    demux->global_metadata = NULL;
  }
  if (demux->mut_ex_streams) {
    g_slist_free (demux->mut_ex_streams);
    demux->mut_ex_streams = NULL;
  }

  demux->state = GST_ASF_DEMUX_STATE_HEADER;
  g_free (demux->objpath);
  demux->objpath = NULL;
  g_strfreev (demux->languages);
  demux->languages = NULL;
  demux->num_languages = 0;
  g_slist_foreach (demux->ext_stream_props, (GFunc) gst_mini_object_unref,
      NULL);
  g_slist_free (demux->ext_stream_props);
  demux->ext_stream_props = NULL;

  while (demux->old_num_streams > 0) {
    gst_asf_demux_free_stream (demux,
        &demux->old_stream[demux->old_num_streams - 1]);
    --demux->old_num_streams;
  }
  memset (demux->old_stream, 0, sizeof (demux->old_stream));
  demux->old_num_streams = 0;

  /* when resetting for a new chained asf, we don't want to remove the pads
   * before adding the new ones */
  if (chain_reset) {
    memcpy (demux->old_stream, demux->stream, sizeof (demux->stream));
    demux->old_num_streams = demux->num_streams;
    demux->num_streams = 0;
  }

  while (demux->num_streams > 0) {
    gst_asf_demux_free_stream (demux, &demux->stream[demux->num_streams - 1]);
    --demux->num_streams;
  }
  memset (demux->stream, 0, sizeof (demux->stream));
  if (!chain_reset) {
    /* do not remove those for not adding pads with same name */
    demux->num_audio_streams = 0;
    demux->num_video_streams = 0;
    demux->have_group_id = FALSE;
    demux->group_id = G_MAXUINT;
  }
  demux->num_streams = 0;
  demux->activated_streams = FALSE;
  demux->first_ts = GST_CLOCK_TIME_NONE;
  demux->segment_ts = GST_CLOCK_TIME_NONE;
  demux->in_gap = 0;
  if (!chain_reset)
    gst_segment_init (&demux->in_segment, GST_FORMAT_UNDEFINED);
  demux->state = GST_ASF_DEMUX_STATE_HEADER;
  demux->seekable = FALSE;
  demux->broadcast = FALSE;
  demux->sidx_interval = 0;
  demux->sidx_num_entries = 0;
  g_free (demux->sidx_entries);
  demux->sidx_entries = NULL;

  demux->speed_packets = 1;

  demux->asf_3D_mode = GST_ASF_3D_NONE;

  if (chain_reset) {
    GST_LOG_OBJECT (demux, "Restarting");
    gst_segment_init (&demux->segment, GST_FORMAT_TIME);
    demux->need_newsegment = TRUE;
    demux->segment_seqnum = 0;
    demux->segment_running = FALSE;
    demux->keyunit_sync = FALSE;
    demux->accurate = FALSE;
    demux->metadata = gst_caps_new_empty ();
    demux->global_metadata = gst_structure_new_empty ("metadata");
    demux->data_size = 0;
    demux->data_offset = 0;
    demux->index_offset = 0;
  } else {
    demux->base_offset = 0;
  }

  g_slist_free (demux->other_streams);
  demux->other_streams = NULL;
}
