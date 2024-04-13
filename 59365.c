gst_asf_demux_handle_src_query (GstPad * pad, GstObject * parent,
    GstQuery * query)
{
  GstASFDemux *demux;
  gboolean res = FALSE;

  demux = GST_ASF_DEMUX (parent);

  GST_DEBUG ("handling %s query",
      gst_query_type_get_name (GST_QUERY_TYPE (query)));

  switch (GST_QUERY_TYPE (query)) {
    case GST_QUERY_DURATION:
    {
      GstFormat format;

      gst_query_parse_duration (query, &format, NULL);

      if (format != GST_FORMAT_TIME) {
        GST_LOG ("only support duration queries in TIME format");
        break;
      }

      res = gst_pad_query_default (pad, parent, query);
      if (!res) {
        GST_OBJECT_LOCK (demux);

        if (demux->segment.duration != GST_CLOCK_TIME_NONE) {
          GST_LOG ("returning duration: %" GST_TIME_FORMAT,
              GST_TIME_ARGS (demux->segment.duration));

          gst_query_set_duration (query, GST_FORMAT_TIME,
              demux->segment.duration);

          res = TRUE;
        } else {
          GST_LOG ("duration not known yet");
        }

        GST_OBJECT_UNLOCK (demux);
      }
      break;
    }

    case GST_QUERY_POSITION:{
      GstFormat format;

      gst_query_parse_position (query, &format, NULL);

      if (format != GST_FORMAT_TIME) {
        GST_LOG ("only support position queries in TIME format");
        break;
      }

      GST_OBJECT_LOCK (demux);

      if (demux->segment.position != GST_CLOCK_TIME_NONE) {
        GST_LOG ("returning position: %" GST_TIME_FORMAT,
            GST_TIME_ARGS (demux->segment.position));

        gst_query_set_position (query, GST_FORMAT_TIME,
            demux->segment.position);

        res = TRUE;
      } else {
        GST_LOG ("position not known yet");
      }

      GST_OBJECT_UNLOCK (demux);
      break;
    }

    case GST_QUERY_SEEKING:{
      GstFormat format;

      gst_query_parse_seeking (query, &format, NULL, NULL, NULL);
      if (format == GST_FORMAT_TIME) {
        gint64 duration;

        GST_OBJECT_LOCK (demux);
        duration = demux->segment.duration;
        GST_OBJECT_UNLOCK (demux);

        if (!demux->streaming || !demux->seekable) {
          gst_query_set_seeking (query, GST_FORMAT_TIME, demux->seekable, 0,
              duration);
          res = TRUE;
        } else {
          GstFormat fmt;
          gboolean seekable;

          /* try upstream first in TIME */
          res = gst_pad_query_default (pad, parent, query);

          gst_query_parse_seeking (query, &fmt, &seekable, NULL, NULL);
          GST_LOG_OBJECT (demux, "upstream %s seekable %d",
              GST_STR_NULL (gst_format_get_name (fmt)), seekable);
          /* if no luck, maybe in BYTES */
          if (!seekable || fmt != GST_FORMAT_TIME) {
            GstQuery *q;

            q = gst_query_new_seeking (GST_FORMAT_BYTES);
            if ((res = gst_pad_peer_query (demux->sinkpad, q))) {
              gst_query_parse_seeking (q, &fmt, &seekable, NULL, NULL);
              GST_LOG_OBJECT (demux, "upstream %s seekable %d",
                  GST_STR_NULL (gst_format_get_name (fmt)), seekable);
              if (fmt != GST_FORMAT_BYTES)
                seekable = FALSE;
            }
            gst_query_unref (q);
            gst_query_set_seeking (query, GST_FORMAT_TIME, seekable, 0,
                duration);
            res = TRUE;
          }
        }
      } else
        GST_LOG_OBJECT (demux, "only support seeking in TIME format");
      break;
    }

    case GST_QUERY_LATENCY:
    {
      gboolean live;
      GstClockTime min, max;

      /* preroll delay does not matter in non-live pipeline,
       * but we might end up in a live (rtsp) one ... */

      /* first forward */
      res = gst_pad_query_default (pad, parent, query);
      if (!res)
        break;

      gst_query_parse_latency (query, &live, &min, &max);

      GST_DEBUG_OBJECT (demux, "Peer latency: live %d, min %"
          GST_TIME_FORMAT " max %" GST_TIME_FORMAT, live,
          GST_TIME_ARGS (min), GST_TIME_ARGS (max));

      GST_OBJECT_LOCK (demux);
      min += demux->latency;
      if (max != -1)
        max += demux->latency;
      GST_OBJECT_UNLOCK (demux);

      gst_query_set_latency (query, live, min, max);
      break;
    }
    case GST_QUERY_SEGMENT:
    {
      GstFormat format;
      gint64 start, stop;

      format = demux->segment.format;

      start =
          gst_segment_to_stream_time (&demux->segment, format,
          demux->segment.start);
      if ((stop = demux->segment.stop) == -1)
        stop = demux->segment.duration;
      else
        stop = gst_segment_to_stream_time (&demux->segment, format, stop);

      gst_query_set_segment (query, demux->segment.rate, format, start, stop);
      res = TRUE;
      break;
    }
    default:
      res = gst_pad_query_default (pad, parent, query);
      break;
  }

  return res;
}
