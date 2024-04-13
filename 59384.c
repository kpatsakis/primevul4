gst_asf_demux_pull_indices (GstASFDemux * demux)
{
  GstBuffer *buf = NULL;
  guint64 offset;
  guint num_read = 0;
  GstFlowReturn ret = GST_FLOW_OK;

  offset = demux->index_offset;

  if (G_UNLIKELY (offset == 0)) {
    GST_DEBUG_OBJECT (demux, "can't read indices, don't know index offset");
    /* non-fatal */
    return GST_FLOW_OK;
  }

  while (gst_asf_demux_pull_data (demux, offset, 16 + 8, &buf, NULL)) {
    AsfObject obj;
    GstMapInfo map;
    guint8 *bufdata;
    guint64 obj_size;

    gst_buffer_map (buf, &map, GST_MAP_READ);
    g_assert (map.size >= 16 + 8);
    if (!asf_demux_peek_object (demux, map.data, 16 + 8, &obj, TRUE)) {
      gst_buffer_unmap (buf, &map);
      gst_buffer_replace (&buf, NULL);
      ret = GST_FLOW_ERROR;
      break;
    }
    gst_buffer_unmap (buf, &map);
    gst_buffer_replace (&buf, NULL);

    /* check for sanity */
    if (G_UNLIKELY (obj.size > (5 * 1024 * 1024))) {
      GST_DEBUG_OBJECT (demux, "implausible index object size, bailing out");
      break;
    }

    if (G_UNLIKELY (!gst_asf_demux_pull_data (demux, offset, obj.size, &buf,
                NULL)))
      break;

    GST_LOG_OBJECT (demux, "index object at offset 0x%" G_GINT64_MODIFIER "X"
        ", size %u", offset, (guint) obj.size);

    offset += obj.size;         /* increase before _process_object changes it */

    gst_buffer_map (buf, &map, GST_MAP_READ);
    g_assert (map.size >= obj.size);
    bufdata = (guint8 *) map.data;
    obj_size = obj.size;
    ret = gst_asf_demux_process_object (demux, &bufdata, &obj_size);
    gst_buffer_unmap (buf, &map);
    gst_buffer_replace (&buf, NULL);

    if (G_UNLIKELY (ret != GST_FLOW_OK))
      break;

    ++num_read;
  }

  GST_DEBUG_OBJECT (demux, "read %u index objects", num_read);
  return ret;
}
