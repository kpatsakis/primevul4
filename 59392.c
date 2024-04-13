gst_asf_demux_setup_pad (GstASFDemux * demux, GstPad * src_pad,
    GstCaps * caps, guint16 id, gboolean is_video, GstBuffer * streamheader,
    GstTagList * tags)
{
  AsfStream *stream;

  gst_pad_use_fixed_caps (src_pad);
  gst_pad_set_caps (src_pad, caps);

  gst_pad_set_event_function (src_pad,
      GST_DEBUG_FUNCPTR (gst_asf_demux_handle_src_event));
  gst_pad_set_query_function (src_pad,
      GST_DEBUG_FUNCPTR (gst_asf_demux_handle_src_query));

  stream = &demux->stream[demux->num_streams];
  stream->caps = caps;
  stream->pad = src_pad;
  stream->id = id;
  stream->fps_known = !is_video;        /* bit hacky for audio */
  stream->is_video = is_video;
  stream->pending_tags = tags;
  stream->discont = TRUE;
  stream->first_buffer = TRUE;
  stream->streamheader = streamheader;
  if (stream->streamheader) {
    stream->streamheader = gst_buffer_make_writable (streamheader);
    GST_BUFFER_FLAG_SET (stream->streamheader, GST_BUFFER_FLAG_HEADER);
  }
  if (is_video) {
    GstStructure *st;
    gint par_x, par_y;
    st = gst_caps_get_structure (caps, 0);
    if (gst_structure_get_fraction (st, "pixel-aspect-ratio", &par_x, &par_y) &&
        par_x > 0 && par_y > 0) {
      GST_DEBUG ("PAR %d/%d", par_x, par_y);
      stream->par_x = par_x;
      stream->par_y = par_y;
    }
  }

  stream->payloads = g_array_new (FALSE, FALSE, sizeof (AsfPayload));

  /* TODO: create this array during reverse play? */
  stream->payloads_rev = g_array_new (FALSE, FALSE, sizeof (AsfPayload));

  GST_INFO ("Created pad %s for stream %u with caps %" GST_PTR_FORMAT,
      GST_PAD_NAME (src_pad), demux->num_streams, caps);

  ++demux->num_streams;

  stream->active = FALSE;

  return stream;
}
