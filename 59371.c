gst_asf_demux_pop_obj (GstASFDemux * demux)
{
  gchar *s;

  if ((s = g_strrstr (demux->objpath, "/"))) {
    *s = '\0';
  } else {
    g_free (demux->objpath);
    demux->objpath = NULL;
  }
}
