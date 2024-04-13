gst_asf_demux_get_uint64 (guint8 ** p_data, guint64 * p_size)
{
  guint64 ret;

  g_assert (*p_size >= 8);
  ret = GST_READ_UINT64_LE (*p_data);
  *p_data += sizeof (guint64);
  *p_size -= sizeof (guint64);
  return ret;
}
