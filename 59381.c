gst_asf_demux_process_queued_extended_stream_objects (GstASFDemux * demux)
{
  GSList *l;
  guint i;

  /* Parse the queued extended stream property objects and add the info
   * to the existing streams or add the new embedded streams, but without
   * activating them yet */
  GST_LOG_OBJECT (demux, "%u queued extended stream properties objects",
      g_slist_length (demux->ext_stream_props));

  for (l = demux->ext_stream_props, i = 0; l != NULL; l = l->next, ++i) {
    GstBuffer *buf = GST_BUFFER (l->data);
    GstMapInfo map;

    gst_buffer_map (buf, &map, GST_MAP_READ);

    GST_LOG_OBJECT (demux, "parsing ext. stream properties object #%u", i);
    gst_asf_demux_process_ext_stream_props (demux, map.data, map.size);
    gst_buffer_unmap (buf, &map);
    gst_buffer_unref (buf);
  }
  g_slist_free (demux->ext_stream_props);
  demux->ext_stream_props = NULL;
}
