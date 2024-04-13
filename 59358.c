gst_asf_demux_get_stream_video (asf_stream_video * video, guint8 ** p_data,
    guint64 * p_size)
{
  if (*p_size < (4 + 4 + 1 + 2))
    return FALSE;

  video->width = gst_asf_demux_get_uint32 (p_data, p_size);
  video->height = gst_asf_demux_get_uint32 (p_data, p_size);
  video->unknown = gst_asf_demux_get_uint8 (p_data, p_size);
  video->size = gst_asf_demux_get_uint16 (p_data, p_size);
  return TRUE;
}
