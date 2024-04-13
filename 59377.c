gst_asf_demux_process_header (GstASFDemux * demux, guint8 * data, guint64 size)
{
  GstFlowReturn ret = GST_FLOW_OK;
  guint32 i, num_objects;
  guint8 unknown G_GNUC_UNUSED;

  /* Get the rest of the header's header */
  if (size < (4 + 1 + 1))
    goto not_enough_data;

  num_objects = gst_asf_demux_get_uint32 (&data, &size);
  unknown = gst_asf_demux_get_uint8 (&data, &size);
  unknown = gst_asf_demux_get_uint8 (&data, &size);

  GST_INFO_OBJECT (demux, "object is a header with %u parts", num_objects);
  demux->saw_file_header = FALSE;
  /* Loop through the header's objects, processing those */
  for (i = 0; i < num_objects; ++i) {
    GST_INFO_OBJECT (demux, "reading header part %u", i);
    ret = gst_asf_demux_process_object (demux, &data, &size);
    if (ret != GST_FLOW_OK) {
      GST_WARNING ("process_object returned %s", gst_asf_get_flow_name (ret));
      break;
    }
  }
  if (!demux->saw_file_header) {
    GST_ELEMENT_ERROR (demux, STREAM, DEMUX, (NULL),
        ("Header does not have mandatory FILE section"));
    return GST_FLOW_ERROR;
  }

  return ret;

not_enough_data:
  {
    GST_ELEMENT_ERROR (demux, STREAM, DEMUX, (NULL),
        ("short read parsing HEADER object"));
    return GST_FLOW_ERROR;
  }
}
