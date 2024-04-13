static MagickBooleanType load_tile_rle(Image *image,Image *tile_image,
  XCFDocInfo *inDocInfo,XCFLayerInfo *inLayerInfo,size_t data_length)
{
  ExceptionInfo
    *exception;

  MagickOffsetType
    size;

  Quantum
    alpha;

  register PixelPacket
    *q;

  size_t
    length;

  ssize_t
    bytes_per_pixel,
    count,
    i,
    j;

  unsigned char
    data,
    pixel,
    *xcfdata,
    *xcfodata,
    *xcfdatalimit;

  bytes_per_pixel=(ssize_t) inDocInfo->bytes_per_pixel;
  xcfdata=(unsigned char *) AcquireQuantumMemory(data_length,sizeof(*xcfdata));
  if (xcfdata == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      image->filename);
  xcfodata=xcfdata;
  count=ReadBlob(image, (size_t) data_length, xcfdata);
  xcfdatalimit = xcfodata+count-1;
  exception=(&image->exception);
  alpha=ScaleCharToQuantum((unsigned char) inLayerInfo->alpha);
  for (i=0; i < (ssize_t) bytes_per_pixel; i++)
  {
    q=GetAuthenticPixels(tile_image,0,0,tile_image->columns,tile_image->rows,
      exception);
    if (q == (PixelPacket *) NULL)
      continue;
    size=(MagickOffsetType) tile_image->rows*tile_image->columns;
    while (size > 0)
    {
      if (xcfdata > xcfdatalimit)
        goto bogus_rle;
      pixel=(*xcfdata++);
      length=(size_t) pixel;
      if (length >= 128)
        {
          length=255-(length-1);
          if (length == 128)
            {
              if (xcfdata >= xcfdatalimit)
                goto bogus_rle;
              length=(size_t) ((*xcfdata << 8) + xcfdata[1]);
              xcfdata+=2;
            }
          size-=length;
          if (size < 0)
            goto bogus_rle;
          if (&xcfdata[length-1] > xcfdatalimit)
            goto bogus_rle;
          while (length-- > 0)
          {
            data=(*xcfdata++);
            switch (i)
            {
              case 0:
              {
                SetPixelRed(q,ScaleCharToQuantum(data));
                if (inDocInfo->image_type == GIMP_GRAY)
                  {
                    SetPixelGreen(q,ScaleCharToQuantum(data));
                    SetPixelBlue(q,ScaleCharToQuantum(data));
                  }
                else
                  {
                    SetPixelGreen(q,GetPixelRed(q));
                    SetPixelBlue(q,GetPixelRed(q));
                  }
                SetPixelAlpha(q,alpha);
                break;
              }
              case 1:
              {
                if (inDocInfo->image_type == GIMP_GRAY)
                  SetPixelAlpha(q,ScaleCharToQuantum(data));
                else
                  SetPixelGreen(q,ScaleCharToQuantum(data));
                break;
              }
              case 2:
              {
                SetPixelBlue(q,ScaleCharToQuantum(data));
                break;
              }
              case 3:
              {
                SetPixelAlpha(q,ScaleCharToQuantum(data));
                break;
              }
            }
            q++;
          }
        }
      else
        {
          length+=1;
          if (length == 128)
            {
              if (xcfdata >= xcfdatalimit)
                goto bogus_rle;
              length=(size_t) ((*xcfdata << 8) + xcfdata[1]);
              xcfdata+=2;
            }
          size-=length;
          if (size < 0)
            goto bogus_rle;
          if (xcfdata > xcfdatalimit)
            goto bogus_rle;
          pixel=(*xcfdata++);
          for (j=0; j < (ssize_t) length; j++)
          {
            data=pixel;
            switch (i)
            {
              case 0:
              {
                SetPixelRed(q,ScaleCharToQuantum(data));
                if (inDocInfo->image_type == GIMP_GRAY)
                  {
                    SetPixelGreen(q,ScaleCharToQuantum(data));
                    SetPixelBlue(q,ScaleCharToQuantum(data));
                  }
                else
                  {
                    SetPixelGreen(q,GetPixelRed(q));
                    SetPixelBlue(q,GetPixelRed(q));
                  }
                SetPixelAlpha(q,alpha);
                break;
              }
              case 1:
              {
                if (inDocInfo->image_type == GIMP_GRAY)
                  SetPixelAlpha(q,ScaleCharToQuantum(data));
                else
                  SetPixelGreen(q,ScaleCharToQuantum(data));
                break;
              }
              case 2:
              {
                SetPixelBlue(q,ScaleCharToQuantum(data));
                break;
              }
              case 3:
              {
                SetPixelAlpha(q,ScaleCharToQuantum(data));
                break;
              }
            }
            q++;
          }
        }
    }
    if (SyncAuthenticPixels(tile_image,exception) == MagickFalse)
      break;
  }
  xcfodata=(unsigned char *) RelinquishMagickMemory(xcfodata);
  return(MagickTrue);

  bogus_rle:
    if (xcfodata != (unsigned char *) NULL)
      xcfodata=(unsigned char *) RelinquishMagickMemory(xcfodata);
  return(MagickFalse);
}
