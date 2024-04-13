static signed short ReadDCMSignedShort(DCMStreamInfo *stream_info,Image *image)
{
  union
  {
    unsigned short
      unsigned_value;

    signed short
      signed_value;
  } quantum;

  quantum.unsigned_value=ReadDCMShort(stream_info,image);
  return(quantum.signed_value);
}
