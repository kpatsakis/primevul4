gst_asf_demux_free_stream (GstASFDemux * demux, AsfStream * stream)
{
  gst_caps_replace (&stream->caps, NULL);
  if (stream->pending_tags) {
    gst_tag_list_unref (stream->pending_tags);
    stream->pending_tags = NULL;
  }
  if (stream->streamheader) {
    gst_buffer_unref (stream->streamheader);
    stream->streamheader = NULL;
  }
  if (stream->pad) {
    if (stream->active) {
      gst_element_remove_pad (GST_ELEMENT_CAST (demux), stream->pad);
      gst_flow_combiner_remove_pad (demux->flowcombiner, stream->pad);
    } else
      gst_object_unref (stream->pad);
    stream->pad = NULL;
  }

  if (stream->payloads) {
    while (stream->payloads->len > 0) {
      AsfPayload *payload;
      guint last;

      last = stream->payloads->len - 1;
      payload = &g_array_index (stream->payloads, AsfPayload, last);
      gst_buffer_replace (&payload->buf, NULL);
      g_array_remove_index (stream->payloads, last);
    }
    g_array_free (stream->payloads, TRUE);
    stream->payloads = NULL;
  }

  if (stream->payloads_rev) {
    while (stream->payloads_rev->len > 0) {
      AsfPayload *payload;
      guint last;

      last = stream->payloads_rev->len - 1;
      payload = &g_array_index (stream->payloads_rev, AsfPayload, last);
      gst_buffer_replace (&payload->buf, NULL);
      g_array_remove_index (stream->payloads_rev, last);
    }
    g_array_free (stream->payloads_rev, TRUE);
    stream->payloads_rev = NULL;
  }

  if (stream->ext_props.valid) {
    g_free (stream->ext_props.payload_extensions);
    stream->ext_props.payload_extensions = NULL;
  }
}
