gst_asf_demux_get_gst_tag_from_tag_name (const gchar * name_utf8)
{
  const struct
  {
    const gchar *asf_name;
    const gchar *gst_name;
  } tags[] = {
    {
    "WM/Genre", GST_TAG_GENRE}, {
    "WM/AlbumTitle", GST_TAG_ALBUM}, {
    "WM/AlbumArtist", GST_TAG_ARTIST}, {
    "WM/Picture", GST_TAG_IMAGE}, {
    "WM/Track", GST_TAG_TRACK_NUMBER}, {
    "WM/TrackNumber", GST_TAG_TRACK_NUMBER}, {
    "WM/Year", GST_TAG_DATE_TIME}
    /* { "WM/Composer", GST_TAG_COMPOSER } */
  };
  gsize out;
  guint i;

  if (name_utf8 == NULL) {
    GST_WARNING ("Failed to convert name to UTF8, skipping");
    return NULL;
  }

  out = strlen (name_utf8);

  for (i = 0; i < G_N_ELEMENTS (tags); ++i) {
    if (strncmp (tags[i].asf_name, name_utf8, out) == 0) {
      GST_LOG ("map tagname '%s' -> '%s'", name_utf8, tags[i].gst_name);
      return tags[i].gst_name;
    }
  }

  return NULL;
}
