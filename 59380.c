gst_asf_demux_process_object (GstASFDemux * demux, guint8 ** p_data,
    guint64 * p_size)
{
  GstFlowReturn ret = GST_FLOW_OK;
  AsfObject obj;
  guint64 obj_data_size;

  if (*p_size < ASF_OBJECT_HEADER_SIZE)
    return ASF_FLOW_NEED_MORE_DATA;

  if (!asf_demux_peek_object (demux, *p_data, ASF_OBJECT_HEADER_SIZE, &obj,
          TRUE))
    return GST_FLOW_ERROR;
  gst_asf_demux_skip_bytes (ASF_OBJECT_HEADER_SIZE, p_data, p_size);

  obj_data_size = obj.size - ASF_OBJECT_HEADER_SIZE;

  if (*p_size < obj_data_size)
    return ASF_FLOW_NEED_MORE_DATA;

  gst_asf_demux_push_obj (demux, obj.id);

  GST_INFO ("%s: size %" G_GUINT64_FORMAT, demux->objpath, obj.size);

  switch (obj.id) {
    case ASF_OBJ_STREAM:
      gst_asf_demux_parse_stream_object (demux, *p_data, obj_data_size);
      ret = GST_FLOW_OK;
      break;
    case ASF_OBJ_FILE:
      ret = gst_asf_demux_process_file (demux, *p_data, obj_data_size);
      break;
    case ASF_OBJ_HEADER:
      ret = gst_asf_demux_process_header (demux, *p_data, obj_data_size);
      break;
    case ASF_OBJ_COMMENT:
      ret = gst_asf_demux_process_comment (demux, *p_data, obj_data_size);
      break;
    case ASF_OBJ_HEAD1:
      ret = gst_asf_demux_process_header_ext (demux, *p_data, obj_data_size);
      break;
    case ASF_OBJ_BITRATE_PROPS:
      ret =
          gst_asf_demux_process_bitrate_props_object (demux, *p_data,
          obj_data_size);
      break;
    case ASF_OBJ_EXT_CONTENT_DESC:
      ret =
          gst_asf_demux_process_ext_content_desc (demux, *p_data,
          obj_data_size);
      break;
    case ASF_OBJ_METADATA_OBJECT:
      ret = gst_asf_demux_process_metadata (demux, *p_data, obj_data_size);
      break;
    case ASF_OBJ_EXTENDED_STREAM_PROPS:{
      GstBuffer *buf;

      /* process these later, we might not have parsed the corresponding
       * stream object yet */
      GST_LOG ("%s: queued for later parsing", demux->objpath);
      buf = gst_buffer_new_and_alloc (obj_data_size);
      gst_buffer_fill (buf, 0, *p_data, obj_data_size);
      demux->ext_stream_props = g_slist_append (demux->ext_stream_props, buf);
      ret = GST_FLOW_OK;
      break;
    }
    case ASF_OBJ_LANGUAGE_LIST:
      ret = gst_asf_demux_process_language_list (demux, *p_data, obj_data_size);
      break;
    case ASF_OBJ_ADVANCED_MUTUAL_EXCLUSION:
      ret = gst_asf_demux_process_advanced_mutual_exclusion (demux, *p_data,
          obj_data_size);
      break;
    case ASF_OBJ_SIMPLE_INDEX:
      ret = gst_asf_demux_process_simple_index (demux, *p_data, obj_data_size);
      break;
    case ASF_OBJ_CONTENT_ENCRYPTION:
    case ASF_OBJ_EXT_CONTENT_ENCRYPTION:
    case ASF_OBJ_DIGITAL_SIGNATURE_OBJECT:
    case ASF_OBJ_UNKNOWN_ENCRYPTION_OBJECT:
      goto error_encrypted;
    case ASF_OBJ_CONCEAL_NONE:
    case ASF_OBJ_HEAD2:
    case ASF_OBJ_UNDEFINED:
    case ASF_OBJ_CODEC_COMMENT:
    case ASF_OBJ_INDEX:
    case ASF_OBJ_PADDING:
    case ASF_OBJ_BITRATE_MUTEX:
    case ASF_OBJ_COMPATIBILITY:
    case ASF_OBJ_INDEX_PLACEHOLDER:
    case ASF_OBJ_INDEX_PARAMETERS:
    case ASF_OBJ_STREAM_PRIORITIZATION:
    case ASF_OBJ_SCRIPT_COMMAND:
    case ASF_OBJ_METADATA_LIBRARY_OBJECT:
    default:
      /* Unknown/unhandled object, skip it and hope for the best */
      GST_INFO ("%s: skipping object", demux->objpath);
      ret = GST_FLOW_OK;
      break;
  }

  /* this can't fail, we checked the number of bytes available before */
  gst_asf_demux_skip_bytes (obj_data_size, p_data, p_size);

  GST_LOG ("%s: ret = %s", demux->objpath, gst_asf_get_flow_name (ret));

  gst_asf_demux_pop_obj (demux);

  return ret;

/* ERRORS */
error_encrypted:
  {
    GST_ELEMENT_ERROR (demux, STREAM, DECRYPT, (NULL), (NULL));
    return GST_FLOW_ERROR;
  }
}
