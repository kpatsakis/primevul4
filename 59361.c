gst_asf_demux_get_uint8 (guint8 ** p_data, guint64 * p_size)
{
  guint8 ret;

  g_assert (*p_size >= 1);
  ret = GST_READ_UINT8 (*p_data);
  *p_data += sizeof (guint8);
  *p_size -= sizeof (guint8);
  return ret;
}
