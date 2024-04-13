static int ati_remote2_lookup(unsigned int hw_code)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ati_remote2_key_table); i++)
		if (ati_remote2_key_table[i].hw_code == hw_code)
			return i;

	return -1;
}
