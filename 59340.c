gst_asf_demux_add_video_stream (GstASFDemux * demux,
    asf_stream_video_format * video, guint16 id,
    guint8 ** p_data, guint64 * p_size)
{
  GstTagList *tags = NULL;
  GstStructure *caps_s;
  GstBuffer *extradata = NULL;
  GstPad *src_pad;
  GstCaps *caps;
  gchar *str;
  gchar *name = NULL;
  gchar *codec_name = NULL;
  guint64 size_left = video->size - 40;
  GstBuffer *streamheader = NULL;
  guint par_w = 1, par_h = 1;

  /* Create the video pad */
  name = g_strdup_printf ("video_%u", demux->num_video_streams);
  src_pad = gst_pad_new_from_static_template (&video_src_template, name);
  g_free (name);

  /* Now try some gstreamer formatted MIME types (from gst_avi_demux_strf_vids) */
  if (size_left) {
    GST_LOG ("Video header has %" G_GUINT64_FORMAT
        " bytes of codec specific data (vs %" G_GUINT64_FORMAT ")", size_left,
        *p_size);
    g_assert (size_left <= *p_size);
    gst_asf_demux_get_buffer (&extradata, size_left, p_data, p_size);
  }

  GST_DEBUG ("video codec %" GST_FOURCC_FORMAT, GST_FOURCC_ARGS (video->tag));

  /* yes, asf_stream_video_format and gst_riff_strf_vids are the same */
  caps = gst_riff_create_video_caps (video->tag, NULL,
      (gst_riff_strf_vids *) video, extradata, NULL, &codec_name);

  if (caps == NULL) {
    caps = gst_caps_new_simple ("video/x-asf-unknown", "fourcc",
        G_TYPE_UINT, video->tag, NULL);
  } else {
    GstStructure *s;
    gint ax, ay;

    s = gst_asf_demux_get_metadata_for_stream (demux, id);
    if (gst_structure_get_int (s, "AspectRatioX", &ax) &&
        gst_structure_get_int (s, "AspectRatioY", &ay) && (ax > 0 && ay > 0)) {
      par_w = ax;
      par_h = ay;
      gst_caps_set_simple (caps, "pixel-aspect-ratio", GST_TYPE_FRACTION,
          ax, ay, NULL);
    } else {
      guint ax, ay;
      /* retry with the global metadata */
      GST_DEBUG ("Retrying with global metadata %" GST_PTR_FORMAT,
          demux->global_metadata);
      s = demux->global_metadata;
      if (gst_structure_get_uint (s, "AspectRatioX", &ax) &&
          gst_structure_get_uint (s, "AspectRatioY", &ay)) {
        GST_DEBUG ("ax:%d, ay:%d", ax, ay);
        if (ax > 0 && ay > 0) {
          par_w = ax;
          par_h = ay;
          gst_caps_set_simple (caps, "pixel-aspect-ratio", GST_TYPE_FRACTION,
              ax, ay, NULL);
        }
      }
    }
    s = gst_caps_get_structure (caps, 0);
    gst_structure_remove_field (s, "framerate");
  }

  caps_s = gst_caps_get_structure (caps, 0);

  /* add format field with fourcc to WMV/VC1 caps to differentiate variants */
  if (gst_structure_has_name (caps_s, "video/x-wmv")) {
    str = g_strdup_printf ("%" GST_FOURCC_FORMAT, GST_FOURCC_ARGS (video->tag));
    gst_caps_set_simple (caps, "format", G_TYPE_STRING, str, NULL);
    g_free (str);

    /* check if h264 has codec_data (avc) or streamheaders (bytestream) */
  } else if (gst_structure_has_name (caps_s, "video/x-h264")) {
    const GValue *value = gst_structure_get_value (caps_s, "codec_data");
    if (value) {
      GstBuffer *buf = gst_value_get_buffer (value);
      GstMapInfo mapinfo;

      if (gst_buffer_map (buf, &mapinfo, GST_MAP_READ)) {
        if (mapinfo.size >= 4 && GST_READ_UINT32_BE (mapinfo.data) == 1) {
          /* this looks like a bytestream start */
          streamheader = gst_buffer_ref (buf);
          gst_asf_demux_add_stream_headers_to_caps (demux, buf, caps_s);
          gst_structure_remove_field (caps_s, "codec_data");
        }

        gst_buffer_unmap (buf, &mapinfo);
      }
    }
  }

  /* For a 3D video, set multiview information into the caps based on
   * what was detected during object parsing */
  if (demux->asf_3D_mode != GST_ASF_3D_NONE) {
    GstVideoMultiviewMode mv_mode = GST_VIDEO_MULTIVIEW_MODE_NONE;
    GstVideoMultiviewFlags mv_flags = GST_VIDEO_MULTIVIEW_FLAGS_NONE;
    const gchar *mview_mode_str;

    switch (demux->asf_3D_mode) {
      case GST_ASF_3D_SIDE_BY_SIDE_HALF_LR:
        mv_mode = GST_VIDEO_MULTIVIEW_MODE_SIDE_BY_SIDE;
        break;
      case GST_ASF_3D_SIDE_BY_SIDE_HALF_RL:
        mv_mode = GST_VIDEO_MULTIVIEW_MODE_SIDE_BY_SIDE;
        mv_flags = GST_VIDEO_MULTIVIEW_FLAGS_RIGHT_VIEW_FIRST;
        break;
      case GST_ASF_3D_TOP_AND_BOTTOM_HALF_LR:
        mv_mode = GST_VIDEO_MULTIVIEW_MODE_TOP_BOTTOM;
        break;
      case GST_ASF_3D_TOP_AND_BOTTOM_HALF_RL:
        mv_mode = GST_VIDEO_MULTIVIEW_MODE_TOP_BOTTOM;
        mv_flags = GST_VIDEO_MULTIVIEW_FLAGS_RIGHT_VIEW_FIRST;
        break;
      case GST_ASF_3D_DUAL_STREAM:{
        gboolean is_right_view = FALSE;
        /* if Advanced_Mutual_Exclusion object exists, use it
         * to figure out which is the left view (lower ID) */
        if (demux->mut_ex_streams != NULL) {
          guint length;
          gint i;

          length = g_slist_length (demux->mut_ex_streams);

          for (i = 0; i < length; i++) {
            gpointer v_s_id;

            v_s_id = g_slist_nth_data (demux->mut_ex_streams, i);

            GST_DEBUG_OBJECT (demux,
                "has Mutual_Exclusion object. stream id in object is %d",
                GPOINTER_TO_INT (v_s_id));

            if (id > GPOINTER_TO_INT (v_s_id))
              is_right_view = TRUE;
          }
        } else {
          /* if the Advaced_Mutual_Exclusion object doesn't exist, assume the
           * first video stream encountered has the lower ID */
          if (demux->num_video_streams > 0) {
            /* This is not the first video stream, assuming right eye view */
            is_right_view = TRUE;
          }
        }
        if (is_right_view)
          mv_mode = GST_VIDEO_MULTIVIEW_MODE_RIGHT;
        else
          mv_mode = GST_VIDEO_MULTIVIEW_MODE_LEFT;
        break;
      }
      default:
        break;
    }

    GST_INFO_OBJECT (demux,
        "stream_id %d, has multiview-mode %d flags 0x%x", id, mv_mode,
        (guint) mv_flags);

    mview_mode_str = gst_video_multiview_mode_to_caps_string (mv_mode);
    if (mview_mode_str != NULL) {
      if (gst_video_multiview_guess_half_aspect (mv_mode, video->width,
              video->height, par_w, par_h))
        mv_flags |= GST_VIDEO_MULTIVIEW_FLAGS_HALF_ASPECT;

      gst_caps_set_simple (caps,
          "multiview-mode", G_TYPE_STRING, mview_mode_str,
          "multiview-flags", GST_TYPE_VIDEO_MULTIVIEW_FLAGSET, mv_flags,
          GST_FLAG_SET_MASK_EXACT, NULL);
    }
  }

  if (codec_name) {
    tags = gst_tag_list_new (GST_TAG_VIDEO_CODEC, codec_name, NULL);
    g_free (codec_name);
  }

  if (extradata)
    gst_buffer_unref (extradata);

  GST_INFO ("Adding video stream #%u, id %u, codec %"
      GST_FOURCC_FORMAT " (0x%08x)", demux->num_video_streams, id,
      GST_FOURCC_ARGS (video->tag), video->tag);

  ++demux->num_video_streams;

  return gst_asf_demux_setup_pad (demux, src_pad, caps, id, TRUE,
      streamheader, tags);
}
