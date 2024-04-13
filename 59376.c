gst_asf_demux_process_file (GstASFDemux * demux, guint8 * data, guint64 size)
{
  guint64 creation_time G_GNUC_UNUSED;
  guint64 file_size G_GNUC_UNUSED;
  guint64 send_time G_GNUC_UNUSED;
  guint64 packets_count, play_time, preroll;
  guint32 flags, min_pktsize, max_pktsize, min_bitrate G_GNUC_UNUSED;

  if (size < (16 + 8 + 8 + 8 + 8 + 8 + 8 + 4 + 4 + 4 + 4))
    goto not_enough_data;

  gst_asf_demux_skip_bytes (16, &data, &size);  /* skip GUID */
  file_size = gst_asf_demux_get_uint64 (&data, &size);
  creation_time = gst_asf_demux_get_uint64 (&data, &size);
  packets_count = gst_asf_demux_get_uint64 (&data, &size);
  play_time = gst_asf_demux_get_uint64 (&data, &size);
  send_time = gst_asf_demux_get_uint64 (&data, &size);
  preroll = gst_asf_demux_get_uint64 (&data, &size);
  flags = gst_asf_demux_get_uint32 (&data, &size);
  min_pktsize = gst_asf_demux_get_uint32 (&data, &size);
  max_pktsize = gst_asf_demux_get_uint32 (&data, &size);
  min_bitrate = gst_asf_demux_get_uint32 (&data, &size);

  demux->broadcast = ! !(flags & 0x01);
  demux->seekable = ! !(flags & 0x02);

  GST_DEBUG_OBJECT (demux, "min_pktsize = %u", min_pktsize);
  GST_DEBUG_OBJECT (demux, "flags::broadcast = %d", demux->broadcast);
  GST_DEBUG_OBJECT (demux, "flags::seekable  = %d", demux->seekable);

  if (demux->broadcast) {
    /* these fields are invalid if the broadcast flag is set */
    play_time = 0;
    file_size = 0;
  }

  if (min_pktsize != max_pktsize)
    goto non_fixed_packet_size;

  demux->packet_size = max_pktsize;

  /* FIXME: do we need send_time as well? what is it? */
  if ((play_time * 100) >= (preroll * GST_MSECOND))
    demux->play_time = (play_time * 100) - (preroll * GST_MSECOND);
  else
    demux->play_time = 0;

  demux->preroll = preroll * GST_MSECOND;

  /* initial latency */
  demux->latency = demux->preroll;

  if (demux->play_time == 0)
    demux->seekable = FALSE;

  GST_DEBUG_OBJECT (demux, "play_time %" GST_TIME_FORMAT,
      GST_TIME_ARGS (demux->play_time));
  GST_DEBUG_OBJECT (demux, "preroll   %" GST_TIME_FORMAT,
      GST_TIME_ARGS (demux->preroll));

  if (demux->play_time > 0) {
    demux->segment.duration = demux->play_time;
  }

  GST_INFO ("object is a file with %" G_GUINT64_FORMAT " data packets",
      packets_count);
  GST_INFO ("preroll = %" G_GUINT64_FORMAT, demux->preroll);

  demux->saw_file_header = TRUE;

  return GST_FLOW_OK;

/* ERRORS */
non_fixed_packet_size:
  {
    GST_ELEMENT_ERROR (demux, STREAM, DEMUX, (NULL),
        ("packet size must be fixed"));
    return GST_FLOW_ERROR;
  }
not_enough_data:
  {
    GST_ELEMENT_ERROR (demux, STREAM, DEMUX, (NULL),
        ("short read parsing FILE object"));
    return GST_FLOW_ERROR;
  }
}
