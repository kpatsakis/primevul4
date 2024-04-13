gst_asf_demux_skip_bytes (guint num_bytes, guint8 ** p_data, guint64 * p_size)
{
  if (*p_size < num_bytes)
    return FALSE;

  *p_data += num_bytes;
  *p_size -= num_bytes;
  return TRUE;
}
