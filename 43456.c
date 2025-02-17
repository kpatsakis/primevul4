static int lz4_uncompress_unknownoutputsize(const char *source, char *dest,
				int isize, size_t maxoutputsize)
{
	const BYTE *ip = (const BYTE *) source;
	const BYTE *const iend = ip + isize;
	const BYTE *ref;


	BYTE *op = (BYTE *) dest;
	BYTE * const oend = op + maxoutputsize;
	BYTE *cpy;

	size_t dec32table[] = {0, 3, 2, 3, 0, 0, 0, 0};
#if LZ4_ARCH64
	size_t dec64table[] = {0, 0, 0, -1, 0, 1, 2, 3};
#endif

	/* Main Loop */
	while (ip < iend) {

		unsigned token;
		size_t length;

		/* get runlength */
		token = *ip++;
		length = (token >> ML_BITS);
		if (length == RUN_MASK) {
			int s = 255;
			while ((ip < iend) && (s == 255)) {
				s = *ip++;
				length += s;
			}
		}
		/* copy literals */
		cpy = op + length;
		if ((cpy > oend - COPYLENGTH) ||
			(ip + length > iend - COPYLENGTH)) {

			if (cpy > oend)
				goto _output_error;/* writes beyond buffer */

			if (ip + length != iend)
				goto _output_error;/*
						    * Error: LZ4 format requires
						    * to consume all input
						    * at this stage
						    */
			memcpy(op, ip, length);
			op += length;
			break;/* Necessarily EOF, due to parsing restrictions */
		}
		LZ4_WILDCOPY(ip, op, cpy);
		ip -= (op - cpy);
		op = cpy;

		/* get offset */
		LZ4_READ_LITTLEENDIAN_16(ref, cpy, ip);
		ip += 2;
		if (ref < (BYTE * const) dest)
			goto _output_error;
			/*
			 * Error : offset creates reference
			 * outside of destination buffer
			 */

		/* get matchlength */
		length = (token & ML_MASK);
		if (length == ML_MASK) {
			while (ip < iend) {
				int s = *ip++;
				length += s;
				if (s == 255)
					continue;
				break;
			}
		}

		/* copy repeated sequence */
		if (unlikely((op - ref) < STEPSIZE)) {
#if LZ4_ARCH64
			size_t dec64 = dec64table[op - ref];
#else
			const int dec64 = 0;
#endif
				op[0] = ref[0];
				op[1] = ref[1];
				op[2] = ref[2];
				op[3] = ref[3];
				op += 4;
				ref += 4;
				ref -= dec32table[op - ref];
				PUT4(ref, op);
				op += STEPSIZE - 4;
				ref -= dec64;
		} else {
			LZ4_COPYSTEP(ref, op);
		}
		cpy = op + length - (STEPSIZE-4);
		if (cpy > oend - COPYLENGTH) {
			if (cpy > oend)
				goto _output_error; /* write outside of buf */

			LZ4_SECURECOPY(ref, op, (oend - COPYLENGTH));
			while (op < cpy)
				*op++ = *ref++;
			op = cpy;
			/*
			 * Check EOF (should never happen, since last 5 bytes
			 * are supposed to be literals)
			 */
			if (op == oend)
				goto _output_error;
			continue;
		}
		LZ4_SECURECOPY(ref, op, cpy);
		op = cpy; /* correction */
	}
	/* end of decoding */
	return (int) (((char *) op) - dest);

	/* write overflow error detected */
_output_error:
	return (int) (-(((char *) ip) - source));
}
