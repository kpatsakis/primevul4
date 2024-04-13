base64_encode (char *to, char *from, unsigned int len)
{
	while (len >= 3)
	{
		three_to_four (from, to);
		len -= 3;
		from += 3;
		to += 4;
	}
	if (len)
	{
		char three[3] = {0,0,0};
		unsigned int i;
		for (i = 0; i < len; i++)
		{
			three[i] = *from++;
		}
		three_to_four (three, to);
		if (len == 1)
		{
			to[2] = to[3] = '=';
		}
		else if (len == 2)
		{
			to[3] = '=';
		}
		to += 4;
	};
	to[0] = 0;
}
