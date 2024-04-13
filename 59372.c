gst_asf_demux_process_advanced_mutual_exclusion (GstASFDemux * demux,
    guint8 * data, guint64 size)
{
  ASFGuid guid;
  guint16 num, i;

  if (size < 16 + 2 + (2 * 2))
    goto not_enough_data;

  gst_asf_demux_get_guid (&guid, &data, &size);
  num = gst_asf_demux_get_uint16 (&data, &size);

  if (num < 2) {
    GST_WARNING_OBJECT (demux, "nonsensical mutually exclusive streams count");
    return GST_FLOW_OK;
  }

  if (size < (num * sizeof (guint16)))
    goto not_enough_data;

  /* read mutually exclusive stream numbers */
  for (i = 0; i < num; ++i) {
    guint8 mes;
    mes = gst_asf_demux_get_uint16 (&data, &size) & 0x7f;
    GST_LOG_OBJECT (demux, "mutually exclusive: stream %d", mes);

    demux->mut_ex_streams =
        g_slist_append (demux->mut_ex_streams, GINT_TO_POINTER (mes));
  }


  return GST_FLOW_OK;

  /* Errors */
not_enough_data:
  {
    GST_WARNING_OBJECT (demux, "short read parsing advanced mutual exclusion");
    return GST_FLOW_OK;         /* not absolutely fatal */
  }
}
