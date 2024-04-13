check_rpcap_heur (tvbuff_t *tvb, gboolean tcp)
{
  gint offset = 0;
  guint8 version, msg_type;
  guint16 msg_value;
  guint32 plen, len, caplen;

  if (tvb_captured_length (tvb) < 8)
    /* Too short */
    return FALSE;

  version = tvb_get_guint8 (tvb, offset);
  if (version != 0)
    /* Incorrect version */
    return FALSE;
  offset++;

  msg_type = tvb_get_guint8 (tvb, offset);
  if (!tcp && msg_type != 7) {
    /* UDP is only used for packets */
    return FALSE;
  }
  if (try_val_to_str(msg_type, message_type) == NULL)
    /* Unknown message type */
    return FALSE;
  offset++;

  msg_value = tvb_get_ntohs (tvb, offset);
  if (msg_value > 0) {
    if (msg_type == RPCAP_MSG_ERROR) {
      /* Must have a valid error code */
      if (try_val_to_str(msg_value, error_codes) == NULL)
        return FALSE;
    } else if (msg_type != RPCAP_MSG_FINDALLIF_REPLY) {
      return FALSE;
    }
  }
  offset += 2;

  plen = tvb_get_ntohl (tvb, offset);
  offset += 4;
  len = (guint32) tvb_reported_length_remaining (tvb, offset);

  switch (msg_type) {

  case RPCAP_MSG_FINDALLIF_REQ:
  case RPCAP_MSG_UPDATEFILTER_REPLY:
  case RPCAP_MSG_AUTH_REPLY:
  case RPCAP_MSG_STATS_REQ:
  case RPCAP_MSG_CLOSE:
  case RPCAP_MSG_SETSAMPLING_REPLY:
  case RPCAP_MSG_ENDCAP_REQ:
  case RPCAP_MSG_ENDCAP_REPLY:
    /* Empty payload */
    if (plen != 0 || len != 0)
      return FALSE;
    break;

  case RPCAP_MSG_OPEN_REPLY:
  case RPCAP_MSG_STARTCAP_REPLY:
  case RPCAP_MSG_SETSAMPLING_REQ:
    /* Always 8 bytes */
    if (plen != 8 || len != 8)
      return FALSE;
    break;

  case RPCAP_MSG_STATS_REPLY:
    /* Always 16 bytes */
    if (plen != 16 || len != 16)
      return FALSE;
    break;

  case RPCAP_MSG_PACKET:
    /* Must have the frame header */
    if (plen < 20)
      return FALSE;

    /* Check if capture length is valid */
    caplen = tvb_get_ntohl (tvb, offset+8);
    /* Always 20 bytes less than packet length */
    if (caplen != (plen - 20) || caplen > 65535)
      return FALSE;
    break;

  case RPCAP_MSG_FINDALLIF_REPLY:
  case RPCAP_MSG_ERROR:
  case RPCAP_MSG_OPEN_REQ:
  case RPCAP_MSG_STARTCAP_REQ:
  case RPCAP_MSG_UPDATEFILTER_REQ:
  case RPCAP_MSG_AUTH_REQ:
    /* Variable length */
    if (plen != len)
      return FALSE;
    break;
  default:
    /* Unknown message type */
    return FALSE;
  }

  return TRUE;
}
