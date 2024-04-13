gst_asf_demux_get_uint32 (guint8 ** p_data, guint64 * p_size)
{
  guint32 ret;

  g_assert (*p_size >= 4);
  ret = GST_READ_UINT32_LE (*p_data);
  *p_data += sizeof (guint32);
  *p_size -= sizeof (guint32);
  return ret;
}
