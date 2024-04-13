static int TIFFMapBlob(thandle_t image,tdata_t *base,toff_t *size)
{
  *base=(tdata_t *) GetBlobStreamData((Image *) image);
  if (*base != (tdata_t *) NULL)
    *size=(toff_t) GetBlobSize((Image *) image);
  if (*base != (tdata_t *) NULL)
    return(1);
  return(0);
}
