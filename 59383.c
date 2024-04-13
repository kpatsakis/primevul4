gst_asf_demux_pull_headers (GstASFDemux * demux, GstFlowReturn * pflow)
{
  GstFlowReturn flow = GST_FLOW_OK;
  AsfObject obj;
  GstBuffer *buf = NULL;
  guint64 size;
  GstMapInfo map;
  guint8 *bufdata;

  GST_LOG_OBJECT (demux, "reading headers");

  /* pull HEADER object header, so we know its size */
  if (!gst_asf_demux_pull_data (demux, demux->base_offset, 16 + 8, &buf, &flow))
    goto read_failed;

  gst_buffer_map (buf, &map, GST_MAP_READ);
  g_assert (map.size >= 16 + 8);
  if (!asf_demux_peek_object (demux, map.data, 16 + 8, &obj, TRUE)) {
    gst_buffer_unmap (buf, &map);
    gst_buffer_replace (&buf, NULL);
    flow = GST_FLOW_ERROR;
    goto read_failed;
  }
  gst_buffer_unmap (buf, &map);
  gst_buffer_replace (&buf, NULL);

  if (obj.id != ASF_OBJ_HEADER)
    goto wrong_type;

  GST_LOG_OBJECT (demux, "header size = %" G_GUINT64_FORMAT, obj.size);

  /* pull HEADER object */
  if (!gst_asf_demux_pull_data (demux, demux->base_offset, obj.size, &buf,
          &flow))
    goto read_failed;

  size = obj.size;              /* don't want obj.size changed */
  gst_buffer_map (buf, &map, GST_MAP_READ);
  g_assert (map.size >= size);
  bufdata = (guint8 *) map.data;
  flow = gst_asf_demux_process_object (demux, &bufdata, &size);
  gst_buffer_unmap (buf, &map);
  gst_buffer_replace (&buf, NULL);

  if (flow != GST_FLOW_OK) {
    GST_WARNING_OBJECT (demux, "process_object: %s", gst_flow_get_name (flow));
    goto parse_failed;
  }

  /* calculate where the packet data starts */
  demux->data_offset = demux->base_offset + obj.size + 50;

  /* now pull beginning of DATA object before packet data */
  if (!gst_asf_demux_pull_data (demux, demux->base_offset + obj.size, 50, &buf,
          &flow))
    goto read_failed;

  gst_buffer_map (buf, &map, GST_MAP_READ);
  g_assert (map.size >= size);
  bufdata = (guint8 *) map.data;
  if (!gst_asf_demux_parse_data_object_start (demux, bufdata))
    goto wrong_type;

  if (demux->num_streams == 0)
    goto no_streams;

  gst_buffer_unmap (buf, &map);
  gst_buffer_replace (&buf, NULL);

  return TRUE;

/* ERRORS */
wrong_type:
  {
    if (buf != NULL) {
      gst_buffer_unmap (buf, &map);
      gst_buffer_replace (&buf, NULL);
    }
    GST_ELEMENT_ERROR (demux, STREAM, WRONG_TYPE, (NULL),
        ("This doesn't seem to be an ASF file"));
    *pflow = GST_FLOW_ERROR;
    return FALSE;
  }

no_streams:
  flow = GST_FLOW_ERROR;
  GST_ELEMENT_ERROR (demux, STREAM, DEMUX, (NULL),
      ("header parsing failed, or no streams found, flow = %s",
          gst_flow_get_name (flow)));
read_failed:
parse_failed:
  {
    if (buf)
      gst_buffer_unmap (buf, &map);
    gst_buffer_replace (&buf, NULL);
    if (flow == ASF_FLOW_NEED_MORE_DATA)
      flow = GST_FLOW_ERROR;
    *pflow = flow;
    return FALSE;
  }
}
