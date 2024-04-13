gst_asf_demux_push_obj (GstASFDemux * demux, guint32 obj_id)
{
  const gchar *nick;

  nick = gst_asf_get_guid_nick (asf_object_guids, obj_id);
  if (g_str_has_prefix (nick, "ASF_OBJ_"))
    nick += strlen ("ASF_OBJ_");

  if (demux->objpath == NULL) {
    demux->objpath = g_strdup (nick);
  } else {
    gchar *newpath;

    newpath = g_strdup_printf ("%s/%s", demux->objpath, nick);
    g_free (demux->objpath);
    demux->objpath = newpath;
  }

  return (const gchar *) demux->objpath;
}
