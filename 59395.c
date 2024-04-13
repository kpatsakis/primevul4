gst_asf_demux_update_caps_from_payload (GstASFDemux * demux, AsfStream * stream)
{
  /* try to determine whether the stream is AC-3 or MPEG; In dvr-ms the codecTag is unreliable
     and often set wrong, inspecting the data is the only way that seem to be working */
  GstTypeFindProbability prob = GST_TYPE_FIND_NONE;
  GstCaps *caps = NULL;
  int i;
  GstAdapter *adapter = gst_adapter_new ();

  for (i = 0; i < stream->payloads->len && prob < GST_TYPE_FIND_LIKELY; ++i) {
    const guint8 *data;
    AsfPayload *payload;
    int len;

    payload = &g_array_index (stream->payloads, AsfPayload, i);
    gst_adapter_push (adapter, gst_buffer_ref (payload->buf));
    len = gst_adapter_available (adapter);
    data = gst_adapter_map (adapter, len);

  again:

#define MIN_LENGTH 128

    /* look for the sync points */
    while (TRUE) {
      if (len < MIN_LENGTH ||   /* give typefind something to work on */
          (data[0] == 0x0b && data[1] == 0x77) ||       /* AC-3 sync point */
          (data[0] == 0xFF && ((data[1] & 0xF0) >> 4) == 0xF))  /* MPEG sync point */
        break;
      ++data;
      --len;
    }

    gst_caps_take (&caps, gst_type_find_helper_for_data (GST_OBJECT (demux),
            data, len, &prob));

    if (prob < GST_TYPE_FIND_LIKELY) {
      ++data;
      --len;
      if (len > MIN_LENGTH)
        /* this wasn't it, look for another sync point */
        goto again;
    }

    gst_adapter_unmap (adapter);
  }

  gst_object_unref (adapter);

  if (caps) {
    gst_caps_take (&stream->caps, caps);
    return TRUE;
  } else {
    return FALSE;
  }
}
