u32 gf_isom_solve_uuid_box(u8 *UUID)
{
	u32 i;
	char strUUID[33], strChar[3];
	strUUID[0] = 0;
	strUUID[32] = 0;
	for (i=0; i<16; i++) {
		snprintf(strChar, 3, "%02X", (unsigned char) UUID[i]);
		strcat(strUUID, strChar);
	}
	if (!strnicmp(strUUID, "8974dbce7be74c5184f97148f9882554", 32))
		return GF_ISOM_BOX_UUID_TENC;
	if (!strnicmp(strUUID, "A5D40B30E81411DDBA2F0800200C9A66", 32))
		return GF_ISOM_BOX_UUID_MSSM;
	if (!strnicmp(strUUID, "D4807EF2CA3946958E5426CB9E46A79F", 32))
		return GF_ISOM_BOX_UUID_TFRF;
	if (!strnicmp(strUUID, "6D1D9B0542D544E680E2141DAFF757B2", 32))
		return GF_ISOM_BOX_UUID_TFXD;
	if (!strnicmp(strUUID, "A2394F525A9B4F14A2446C427C648DF4", 32))
		return GF_ISOM_BOX_UUID_PSEC;
	if (!strnicmp(strUUID, "D08A4F1810F34A82B6C832D8ABA183D3", 32))
		return GF_ISOM_BOX_UUID_PSSH;

	return 0;
}