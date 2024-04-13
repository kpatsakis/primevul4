gst_asf_demux_activate_stream (GstASFDemux * demux, AsfStream * stream)
{
  if (!stream->active) {
    GstEvent *event;
    gchar *stream_id;

    GST_INFO_OBJECT (demux, "Activating stream %2u, pad %s, caps %"
        GST_PTR_FORMAT, stream->id, GST_PAD_NAME (stream->pad), stream->caps);
    gst_pad_set_active (stream->pad, TRUE);

    stream_id =
        gst_pad_create_stream_id_printf (stream->pad, GST_ELEMENT_CAST (demux),
        "%03u", stream->id);

    event =
        gst_pad_get_sticky_event (demux->sinkpad, GST_EVENT_STREAM_START, 0);
    if (event) {
      if (gst_event_parse_group_id (event, &demux->group_id))
        demux->have_group_id = TRUE;
      else
        demux->have_group_id = FALSE;
      gst_event_unref (event);
    } else if (!demux->have_group_id) {
      demux->have_group_id = TRUE;
      demux->group_id = gst_util_group_id_next ();
    }

    event = gst_event_new_stream_start (stream_id);
    if (demux->have_group_id)
      gst_event_set_group_id (event, demux->group_id);

    gst_pad_push_event (stream->pad, event);
    g_free (stream_id);
    gst_pad_set_caps (stream->pad, stream->caps);

    gst_element_add_pad (GST_ELEMENT_CAST (demux), stream->pad);
    gst_flow_combiner_add_pad (demux->flowcombiner, stream->pad);
    stream->active = TRUE;
  }
}
