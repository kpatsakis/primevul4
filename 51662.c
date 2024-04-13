static int ReadDCMByte(DCMStreamInfo *stream_info,Image *image)
{
  if (image->compression != RLECompression)
    return(ReadBlobByte(image));
  if (stream_info->count == 0)
    {
      int
        byte;

      ssize_t
        count;

      if (stream_info->remaining <= 2)
        stream_info->remaining=0;
      else
        stream_info->remaining-=2;
      count=(ssize_t) ReadBlobByte(image);
      byte=ReadBlobByte(image);
      if (count == 128)
        return(0);
      else
        if (count < 128)
          {
            /*
              Literal bytes.
            */
            stream_info->count=count;
            stream_info->byte=(-1);
            return(byte);
          }
        else
          {
            /*
              Repeated bytes.
            */
            stream_info->count=256-count;
            stream_info->byte=byte;
            return(byte);
          }
    }
  stream_info->count--;
  if (stream_info->byte >= 0)
    return(stream_info->byte);
  if (stream_info->remaining > 0)
    stream_info->remaining--;
  return(ReadBlobByte(image));
}
