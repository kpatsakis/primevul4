static MagickBooleanType ReadDDSInfo(Image *image, DDSInfo *dds_info)
{
  size_t
    hdr_size,
    required;

  /* Seek to start of header */
  (void) SeekBlob(image, 4, SEEK_SET);

  /* Check header field */
  hdr_size = ReadBlobLSBLong(image);
  if (hdr_size != 124)
    return MagickFalse;

  /* Fill in DDS info struct */
  dds_info->flags = ReadBlobLSBLong(image);

  /* Check required flags */
  required=(size_t) (DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT);
  if ((dds_info->flags & required) != required)
    return MagickFalse;

  dds_info->height = ReadBlobLSBLong(image);
  dds_info->width = ReadBlobLSBLong(image);
  dds_info->pitchOrLinearSize = ReadBlobLSBLong(image);
  dds_info->depth = ReadBlobLSBLong(image);
  dds_info->mipmapcount = ReadBlobLSBLong(image);

  (void) SeekBlob(image, 44, SEEK_CUR);   /* reserved region of 11 DWORDs */

  /* Read pixel format structure */
  hdr_size = ReadBlobLSBLong(image);
  if (hdr_size != 32)
    return MagickFalse;

  dds_info->pixelformat.flags = ReadBlobLSBLong(image);
  dds_info->pixelformat.fourcc = ReadBlobLSBLong(image);
  dds_info->pixelformat.rgb_bitcount = ReadBlobLSBLong(image);
  dds_info->pixelformat.r_bitmask = ReadBlobLSBLong(image);
  dds_info->pixelformat.g_bitmask = ReadBlobLSBLong(image);
  dds_info->pixelformat.b_bitmask = ReadBlobLSBLong(image);
  dds_info->pixelformat.alpha_bitmask = ReadBlobLSBLong(image);

  dds_info->ddscaps1 = ReadBlobLSBLong(image);
  dds_info->ddscaps2 = ReadBlobLSBLong(image);
  (void) SeekBlob(image, 12, SEEK_CUR); /* 3 reserved DWORDs */

  return MagickTrue;
}
