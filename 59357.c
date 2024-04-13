gst_asf_demux_get_stream_audio (asf_stream_audio * audio, guint8 ** p_data,
    guint64 * p_size)
{
  if (*p_size < (2 + 2 + 4 + 4 + 2 + 2 + 2))
    return FALSE;

  /* WAVEFORMATEX Structure */
  audio->codec_tag = gst_asf_demux_get_uint16 (p_data, p_size);
  audio->channels = gst_asf_demux_get_uint16 (p_data, p_size);
  audio->sample_rate = gst_asf_demux_get_uint32 (p_data, p_size);
  audio->byte_rate = gst_asf_demux_get_uint32 (p_data, p_size);
  audio->block_align = gst_asf_demux_get_uint16 (p_data, p_size);
  audio->word_size = gst_asf_demux_get_uint16 (p_data, p_size);
  /* Codec specific data size */
  audio->size = gst_asf_demux_get_uint16 (p_data, p_size);
  if (audio->size > *p_size) {
    GST_WARNING ("Corrupted audio codec_data (should be at least %u bytes, is %"
        G_GUINT64_FORMAT " long)", audio->size, *p_size);
    return FALSE;
  }
  return TRUE;
}
