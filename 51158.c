php_mysqlnd_net_store_length(zend_uchar *packet, uint64_t length)
{
	if (length < (uint64_t) L64(251)) {
		*packet = (zend_uchar) length;
		return packet + 1;
	}

	if (length < (uint64_t) L64(65536)) {
		*packet++ = 252;
		int2store(packet,(unsigned int) length);
		return packet + 2;
	}

	if (length < (uint64_t) L64(16777216)) {
		*packet++ = 253;
		int3store(packet,(ulong) length);
		return packet + 3;
	}
	*packet++ = 254;
	int8store(packet, length);
	return packet + 8;
}
