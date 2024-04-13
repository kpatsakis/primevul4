gst_asf_demux_get_guid (ASFGuid * guid, guint8 ** p_data, guint64 * p_size)
{
  g_assert (*p_size >= 4 * sizeof (guint32));

  guid->v1 = gst_asf_demux_get_uint32 (p_data, p_size);
  guid->v2 = gst_asf_demux_get_uint32 (p_data, p_size);
  guid->v3 = gst_asf_demux_get_uint32 (p_data, p_size);
  guid->v4 = gst_asf_demux_get_uint32 (p_data, p_size);
}
