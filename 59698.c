size_t base64encode(char *outbuf, const unsigned char *buf, size_t size)
{
	if (!outbuf || !buf || (size <= 0)) {
		return 0;
	}

	size_t n = 0;
	size_t m = 0;
	unsigned char input[3];
	unsigned int output[4];
	while (n < size) {
		input[0] = buf[n];
		input[1] = (n+1 < size) ? buf[n+1] : 0;
		input[2] = (n+2 < size) ? buf[n+2] : 0;
		output[0] = input[0] >> 2;
		output[1] = ((input[0] & 3) << 4) + (input[1] >> 4);
		output[2] = ((input[1] & 15) << 2) + (input[2] >> 6);
		output[3] = input[2] & 63;
		outbuf[m++] = base64_str[(int)output[0]];
		outbuf[m++] = base64_str[(int)output[1]];
		outbuf[m++] = (n+1 < size) ? base64_str[(int)output[2]] : base64_pad;
		outbuf[m++] = (n+2 < size) ? base64_str[(int)output[3]] : base64_pad;
		n+=3;
	}
	outbuf[m] = 0; // 0-termination!
 	return m;
 }
