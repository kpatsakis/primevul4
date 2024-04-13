static char *createAccessFlagStr(ut32 flags, AccessFor forWhat) {
	#define NUM_FLAGS 18
	static const char* kAccessStrings[kAccessForMAX][NUM_FLAGS] = {
		{
			/* class, inner class */
			"PUBLIC",           /* 0x0001 */
			"PRIVATE",          /* 0x0002 */
			"PROTECTED",        /* 0x0004 */
			"STATIC",           /* 0x0008 */
			"FINAL",            /* 0x0010 */
			"?",                /* 0x0020 */
			"?",                /* 0x0040 */
			"?",                /* 0x0080 */
			"?",                /* 0x0100 */
			"INTERFACE",        /* 0x0200 */
			"ABSTRACT",         /* 0x0400 */
			"?",                /* 0x0800 */
			"SYNTHETIC",        /* 0x1000 */
			"ANNOTATION",       /* 0x2000 */
			"ENUM",             /* 0x4000 */
			"?",                /* 0x8000 */
			"VERIFIED",         /* 0x10000 */
			"OPTIMIZED",        /* 0x20000 */
		},
		{
			/* method */
			"PUBLIC",           /* 0x0001 */
			"PRIVATE",          /* 0x0002 */
			"PROTECTED",        /* 0x0004 */
			"STATIC",           /* 0x0008 */
			"FINAL",            /* 0x0010 */
			"SYNCHRONIZED",     /* 0x0020 */
			"BRIDGE",           /* 0x0040 */
			"VARARGS",          /* 0x0080 */
			"NATIVE",           /* 0x0100 */
			"?",                /* 0x0200 */
			"ABSTRACT",         /* 0x0400 */
			"STRICT",           /* 0x0800 */
			"SYNTHETIC",        /* 0x1000 */
			"?",                /* 0x2000 */
			"?",                /* 0x4000 */
			"MIRANDA",          /* 0x8000 */
			"CONSTRUCTOR",      /* 0x10000 */
			"DECLARED_SYNCHRONIZED", /* 0x20000 */
		},
		{
			/* field */
			"PUBLIC",           /* 0x0001 */
			"PRIVATE",          /* 0x0002 */
			"PROTECTED",        /* 0x0004 */
			"STATIC",           /* 0x0008 */
			"FINAL",            /* 0x0010 */
			"?",                /* 0x0020 */
			"VOLATILE",         /* 0x0040 */
			"TRANSIENT",        /* 0x0080 */
			"?",                /* 0x0100 */
			"?",                /* 0x0200 */
			"?",                /* 0x0400 */
			"?",                /* 0x0800 */
			"SYNTHETIC",        /* 0x1000 */
			"?",                /* 0x2000 */
			"ENUM",             /* 0x4000 */
			"?",                /* 0x8000 */
			"?",                /* 0x10000 */
			"?",                /* 0x20000 */
		},
	};
	const int kLongest = 21;
	int i, count;
	char* str;
	char* cp;
	count = countOnes(flags);
	cp = str = (char*) malloc (count * (kLongest + 1) + 1);
	for (i = 0; i < NUM_FLAGS; i++) {
		if (flags & 0x01) {
			const char* accessStr = kAccessStrings[forWhat][i];
			int len = strlen(accessStr);
			if (cp != str) {
				*cp++ = ' ';
			}
			memcpy(cp, accessStr, len);
			cp += len;
		}
		flags >>= 1;
	}
	*cp = '\0';
	return str;
}
