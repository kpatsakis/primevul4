gst_asf_demux_add_global_tags (GstASFDemux * demux, GstTagList * taglist)
{
  GstTagList *t;

  GST_DEBUG_OBJECT (demux, "adding global tags: %" GST_PTR_FORMAT, taglist);

  if (taglist == NULL)
    return;

  if (gst_tag_list_is_empty (taglist)) {
    gst_tag_list_unref (taglist);
    return;
  }

  t = gst_tag_list_merge (demux->taglist, taglist, GST_TAG_MERGE_APPEND);
  gst_tag_list_set_scope (t, GST_TAG_SCOPE_GLOBAL);
  if (demux->taglist)
    gst_tag_list_unref (demux->taglist);
  gst_tag_list_unref (taglist);
  demux->taglist = t;
  GST_LOG_OBJECT (demux, "global tags now: %" GST_PTR_FORMAT, demux->taglist);
}
