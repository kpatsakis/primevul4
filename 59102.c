static __u32 hash( const char* name)
{
	__u32 h = 0;
	__u32 g;

	while(*name) {
		h = (h<<4) + *name++;
		if ((g = (h & 0xf0000000)))
			h ^=g>>24;
		h &=~g;
	}
	return h;
}
