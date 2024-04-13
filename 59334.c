asf_demux_parse_picture_tag (GstTagList * tags, const guint8 * tag_data,
    guint tag_data_len)
{
  GstByteReader r;
  const guint8 *img_data = NULL;
  guint32 img_data_len = 0;
  guint8 pic_type = 0;

  gst_byte_reader_init (&r, tag_data, tag_data_len);

  /* skip mime type string (we don't trust it and do our own typefinding),
   * and also skip the description string, since we don't use it */
  if (!gst_byte_reader_get_uint8 (&r, &pic_type) ||
      !gst_byte_reader_get_uint32_le (&r, &img_data_len) ||
      !gst_byte_reader_skip_string_utf16 (&r) ||
      !gst_byte_reader_skip_string_utf16 (&r) ||
      !gst_byte_reader_get_data (&r, img_data_len, &img_data)) {
    goto not_enough_data;
  }


  if (!gst_tag_list_add_id3_image (tags, img_data, img_data_len, pic_type))
    GST_DEBUG ("failed to add image extracted from WM/Picture tag to taglist");

  return;

not_enough_data:
  {
    GST_DEBUG ("Failed to read WM/Picture tag: not enough data");
    GST_MEMDUMP ("WM/Picture data", tag_data, tag_data_len);
    return;
  }
}
