static MagickBooleanType ReadDCMPixels(Image *image,DCMInfo *info,
  DCMStreamInfo *stream_info,MagickBooleanType first_segment,
  ExceptionInfo *exception)
{
  int
    byte,
    index;

  MagickBooleanType
    status;

  LongPixelPacket
    pixel;

  register ssize_t
    i,
    x;

  register IndexPacket
    *indexes;

  register PixelPacket
    *q;

  ssize_t
    y;

  /*
    Convert DCM Medical image to pixel packets.
  */
  byte=0;
  i=0;
  status=MagickTrue;

  for (y=0; y < (ssize_t) image->rows; y++)
  {
    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetAuthenticIndexQueue(image);
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if (info->samples_per_pixel == 1)
        {
          int
            pixel_value;

          if (info->bytes_per_pixel == 1)
            pixel_value=info->polarity != MagickFalse ? ((int) info->max_value-
              ReadDCMByte(stream_info,image)) :
              ReadDCMByte(stream_info,image);
          else
            if ((info->bits_allocated != 12) || (info->significant_bits != 12))
              {
                if (info->signed_data)
                  pixel_value=ReadDCMSignedShort(stream_info,image);
                else
                  pixel_value=(int) ReadDCMShort(stream_info,image);
                if (info->polarity != MagickFalse)
                  pixel_value=(int) info->max_value-pixel_value;
              }
            else
              {
                if ((i & 0x01) != 0)
                  pixel_value=(ReadDCMByte(stream_info,image) << 8) |
                    byte;
                else
                  {
                    pixel_value=ReadDCMSignedShort(stream_info,image);
                    byte=(int) (pixel_value & 0x0f);
                    pixel_value>>=4;
                  }
                i++;
              }
          if (info->signed_data == 1)
            pixel_value-=32767;
          if (info->rescale)
            {
              double
                scaled_value;

              scaled_value=pixel_value*info->rescale_slope+
                info->rescale_intercept;
              if (info->window_width == 0)
                {
                  index=(int) scaled_value;
                }
              else
                {
                  double
                    window_max,
                    window_min;
    
                  window_min=ceil(info->window_center-
                    (info->window_width-1.0)/2.0-0.5);
                  window_max=floor(info->window_center+
                    (info->window_width-1.0)/2.0+0.5);
                  if (scaled_value <= window_min)
                    index=0;
                  else
                    if (scaled_value > window_max)
                      index=(int) info->max_value;
                    else
                      index=(int) (info->max_value*(((scaled_value-
                        info->window_center-0.5)/(info->window_width-1))+0.5));
                }
            }
          else
            {
              index=pixel_value;
            }
          index&=info->mask;
          index=(int) ConstrainColormapIndex(image,(size_t) index);
          if (first_segment != MagickFalse)
            SetPixelIndex(indexes+x,index);
          else
            SetPixelIndex(indexes+x,(((size_t) index) |
              (((size_t) GetPixelIndex(indexes+x)) << 8)));
          pixel.red=1U*image->colormap[index].red;
          pixel.green=1U*image->colormap[index].green;
          pixel.blue=1U*image->colormap[index].blue;
        }
      else
        {
          if (info->bytes_per_pixel == 1)
            {
              pixel.red=(unsigned int) ReadDCMByte(stream_info,image);
              pixel.green=(unsigned int) ReadDCMByte(stream_info,image);
              pixel.blue=(unsigned int) ReadDCMByte(stream_info,image);
            }
          else
            {
              pixel.red=ReadDCMShort(stream_info,image);
              pixel.green=ReadDCMShort(stream_info,image);
              pixel.blue=ReadDCMShort(stream_info,image);
            }
          pixel.red&=info->mask;
          pixel.green&=info->mask;
          pixel.blue&=info->mask;
          if (info->scale != (Quantum *) NULL)
            {
              if ((MagickSizeType) pixel.red <= GetQuantumRange(info->depth))
                pixel.red=info->scale[pixel.red];
              if ((MagickSizeType) pixel.green <= GetQuantumRange(info->depth))
                pixel.green=info->scale[pixel.green];
              if ((MagickSizeType) pixel.blue <= GetQuantumRange(info->depth))
                pixel.blue=info->scale[pixel.blue];
            }
        }
      if (first_segment != MagickFalse)
        {
          SetPixelRed(q,pixel.red);
          SetPixelGreen(q,pixel.green);
          SetPixelBlue(q,pixel.blue);
        }
      else
        {
          SetPixelRed(q,(((size_t) pixel.red) |
            (((size_t) GetPixelRed(q)) << 8)));
          SetPixelGreen(q,(((size_t) pixel.green) |
            (((size_t) GetPixelGreen(q)) << 8)));
          SetPixelBlue(q,(((size_t) pixel.blue) |
            (((size_t) GetPixelBlue(q)) << 8)));
        }
      q++;
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
    if (image->previous == (Image *) NULL)
      {
        status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
          image->rows);
        if (status == MagickFalse)
          break;
      }
  }
  return(status);
}
