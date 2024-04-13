static bool ims_pcu_byte_needs_escape(u8 byte)
{
	return byte == IMS_PCU_PROTOCOL_STX ||
	       byte == IMS_PCU_PROTOCOL_ETX ||
	       byte == IMS_PCU_PROTOCOL_DLE;
}
