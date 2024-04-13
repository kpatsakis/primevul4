NeXTPreDecode(TIFF* tif, uint16 s)
{
	static const char module[] = "NeXTPreDecode";
	TIFFDirectory *td = &tif->tif_dir;
	(void)s;

	if( td->td_bitspersample != 2 )
	{
		TIFFErrorExt(tif->tif_clientdata, module, "Unsupported BitsPerSample = %d",
					 td->td_bitspersample);
		return (0);
	}
	return (1);
}
