static int satsub64be(const unsigned char *v1,const unsigned char *v2)
{	int ret,sat,brw,i;

	if (sizeof(long) == 8) do
	{	const union { long one; char little; } is_endian = {1};
		long l;

		if (is_endian.little)			break;
		/* not reached on little-endians */
		/* following test is redundant, because input is
		 * always aligned, but I take no chances... */
		if (((size_t)v1|(size_t)v2)&0x7)	break;

		l  = *((long *)v1);
		l -= *((long *)v2);
		if (l>128)		return 128;
		else if (l<-128)	return -128;
		else			return (int)l;
	} while (0);

	ret = (int)v1[7]-(int)v2[7];
	sat = 0;
	brw = ret>>8;	/* brw is either 0 or -1 */
	if (ret & 0x80)
	{	for (i=6;i>=0;i--)
		{	brw += (int)v1[i]-(int)v2[i];
			sat |= ~brw;
			brw >>= 8;
		}
	}
	else
	{	for (i=6;i>=0;i--)
		{	brw += (int)v1[i]-(int)v2[i];
			sat |= brw;
			brw >>= 8;
		}
	}
	brw <<= 8;	/* brw is either 0 or -256 */

	if (sat&0xff)	return brw | 0x80;
	else		return brw + (ret&0xFF);
}
