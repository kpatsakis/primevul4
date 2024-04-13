gst_asf_demux_get_bytes (guint8 ** p_buf, guint num_bytes_to_read,
    guint8 ** p_data, guint64 * p_size)
{
  *p_buf = NULL;

  if (*p_size < num_bytes_to_read)
    return FALSE;

  *p_buf = g_memdup (*p_data, num_bytes_to_read);
  *p_data += num_bytes_to_read;
  *p_size -= num_bytes_to_read;
  return TRUE;
}
