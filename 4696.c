void fmtutil_guid_to_uuid(u8 *id)
{
	u8 tmp[16];
	de_memcpy(tmp, id, 16);
	id[0] = tmp[3]; id[1] = tmp[2]; id[2] = tmp[1]; id[3] = tmp[0];
	id[4] = tmp[5]; id[5] = tmp[4];
	id[6] = tmp[7]; id[7] = tmp[6];
}