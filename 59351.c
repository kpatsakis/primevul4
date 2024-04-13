gst_asf_demux_get_buffer (GstBuffer ** p_buf, guint num_bytes_to_read,
    guint8 ** p_data, guint64 * p_size)
{
  *p_buf = NULL;

  if (*p_size < num_bytes_to_read)
    return FALSE;

  *p_buf = gst_buffer_new_and_alloc (num_bytes_to_read);
  gst_buffer_fill (*p_buf, 0, *p_data, num_bytes_to_read);

  *p_data += num_bytes_to_read;
  *p_size -= num_bytes_to_read;

  return TRUE;
}
