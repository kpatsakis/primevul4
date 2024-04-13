static u8 scale_7_to_255(u8 x)
{
	return (u8)(0.5+(((double)x)*(255.0/7.0)));
}