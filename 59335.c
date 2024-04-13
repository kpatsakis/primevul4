asf_demux_peek_object (GstASFDemux * demux, const guint8 * data,
    guint data_len, AsfObject * object, gboolean expect)
{
  ASFGuid guid;

  /* Callers should have made sure that data_len is big enough */
  g_assert (data_len >= ASF_OBJECT_HEADER_SIZE);

  if (data_len < ASF_OBJECT_HEADER_SIZE)
    return FALSE;

  guid.v1 = GST_READ_UINT32_LE (data + 0);
  guid.v2 = GST_READ_UINT32_LE (data + 4);
  guid.v3 = GST_READ_UINT32_LE (data + 8);
  guid.v4 = GST_READ_UINT32_LE (data + 12);

  /* FIXME: make asf_demux_identify_object_guid() */
  object->id = gst_asf_demux_identify_guid (asf_object_guids, &guid);
  if (object->id == ASF_OBJ_UNDEFINED && expect) {
    GST_WARNING_OBJECT (demux, "Unknown object %08x-%08x-%08x-%08x",
        guid.v1, guid.v2, guid.v3, guid.v4);
  }

  object->size = GST_READ_UINT64_LE (data + 16);
  if (object->id != ASF_OBJ_DATA && object->size >= G_MAXUINT) {
    GST_WARNING_OBJECT (demux,
        "ASF Object size corrupted (greater than 32bit)");
    return FALSE;
  }


  return TRUE;
}
