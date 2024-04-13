get_rpcap_pdu_len (packet_info *pinfo _U_, tvbuff_t *tvb, int offset, void *data _U_)
{
  return tvb_get_ntohl (tvb, offset + 4) + 8;
}
