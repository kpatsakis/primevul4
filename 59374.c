gst_asf_demux_process_comment (GstASFDemux * demux, guint8 * data, guint64 size)
{
  struct
  {
    const gchar *gst_tag;
    guint16 val_length;
    gchar *val_utf8;
  } tags[5] = {
    {
    GST_TAG_TITLE, 0, NULL}, {
    GST_TAG_ARTIST, 0, NULL}, {
    GST_TAG_COPYRIGHT, 0, NULL}, {
    GST_TAG_DESCRIPTION, 0, NULL}, {
    GST_TAG_COMMENT, 0, NULL}
  };
  GstTagList *taglist;
  GValue value = { 0 };
  gsize in, out;
  gint i = -1;

  GST_INFO_OBJECT (demux, "object is a comment");

  if (size < (2 + 2 + 2 + 2 + 2))
    goto not_enough_data;

  tags[0].val_length = gst_asf_demux_get_uint16 (&data, &size);
  tags[1].val_length = gst_asf_demux_get_uint16 (&data, &size);
  tags[2].val_length = gst_asf_demux_get_uint16 (&data, &size);
  tags[3].val_length = gst_asf_demux_get_uint16 (&data, &size);
  tags[4].val_length = gst_asf_demux_get_uint16 (&data, &size);

  GST_DEBUG_OBJECT (demux, "Comment lengths: title=%d author=%d copyright=%d "
      "description=%d rating=%d", tags[0].val_length, tags[1].val_length,
      tags[2].val_length, tags[3].val_length, tags[4].val_length);

  for (i = 0; i < G_N_ELEMENTS (tags); ++i) {
    if (size < tags[i].val_length)
      goto not_enough_data;

    /* might be just '/0', '/0'... */
    if (tags[i].val_length > 2 && tags[i].val_length % 2 == 0) {
      /* convert to UTF-8 */
      tags[i].val_utf8 = g_convert ((gchar *) data, tags[i].val_length,
          "UTF-8", "UTF-16LE", &in, &out, NULL);
    }
    gst_asf_demux_skip_bytes (tags[i].val_length, &data, &size);
  }

  /* parse metadata into taglist */
  taglist = gst_tag_list_new_empty ();
  g_value_init (&value, G_TYPE_STRING);
  for (i = 0; i < G_N_ELEMENTS (tags); ++i) {
    if (tags[i].val_utf8 && strlen (tags[i].val_utf8) > 0 && tags[i].gst_tag) {
      g_value_set_string (&value, tags[i].val_utf8);
      gst_tag_list_add_values (taglist, GST_TAG_MERGE_APPEND,
          tags[i].gst_tag, &value, NULL);
    }
  }
  g_value_unset (&value);

  gst_asf_demux_add_global_tags (demux, taglist);

  for (i = 0; i < G_N_ELEMENTS (tags); ++i)
    g_free (tags[i].val_utf8);

  return GST_FLOW_OK;

not_enough_data:
  {
    GST_WARNING_OBJECT (demux, "unexpectedly short of data while processing "
        "comment tag section %d, skipping comment object", i);
    for (i = 0; i < G_N_ELEMENTS (tags); i++)
      g_free (tags[i].val_utf8);
    return GST_FLOW_OK;         /* not really fatal */
  }
}
