static void WriteFourCC(Image *image, const size_t compression,
  const MagickBooleanType clusterFit, const MagickBooleanType weightByAlpha,
  ExceptionInfo *exception)
{
  register const PixelPacket
    *p;

  register ssize_t
    x;

  ssize_t
    i,
    y,
    bx,
    by;

  for (y=0; y < (ssize_t) image->rows; y+=4)
  {
    for (x=0; x < (ssize_t) image->columns; x+=4)
    {
      MagickBooleanType
        match;

      DDSVector4
        point,
        points[16];

      size_t
        count = 0,
        max5 = 0,
        max7 = 0,
        min5 = 255,
        min7 = 255,
        columns = 4,
        rows = 4;

      ssize_t
        alphas[16],
        map[16];

      unsigned char
        alpha;

      if (x + columns >= image->columns)
        columns = image->columns - x;

      if (y + rows >= image->rows)
        rows = image->rows - y;

      p=GetVirtualPixels(image,x,y,columns,rows,exception);
      if (p == (const PixelPacket *) NULL)
        break;

      for (i=0; i<16; i++)
      {
        map[i] = -1;
        alphas[i] = -1;
      }

      for (by=0; by < (ssize_t) rows; by++)
      {
        for (bx=0; bx < (ssize_t) columns; bx++)
        {
          if (compression == FOURCC_DXT5)
            alpha = ScaleQuantumToChar(GetPixelAlpha(p));
          else
            alpha = 255;

          if (compression == FOURCC_DXT5)
            {
              if (alpha < min7)
                min7 = alpha;
              if (alpha > max7)
                max7 = alpha;
              if (alpha != 0 && alpha < min5)
                min5 = alpha;
              if (alpha != 255 && alpha > max5)
                max5 = alpha;
            }
          
          alphas[4*by + bx] = (size_t)alpha;

          point.x = (float)ScaleQuantumToChar(GetPixelRed(p)) / 255.0f;
          point.y = (float)ScaleQuantumToChar(GetPixelGreen(p)) / 255.0f;
          point.z = (float)ScaleQuantumToChar(GetPixelBlue(p)) / 255.0f;
          point.w = weightByAlpha ? (float)(alpha + 1) / 256.0f : 1.0f;
          p++;

          match = MagickFalse;
          for (i=0; i < (ssize_t) count; i++)
          {
            if ((points[i].x == point.x) &&
                (points[i].y == point.y) &&
                (points[i].z == point.z) &&
                (alpha       >= 128 || compression == FOURCC_DXT5))
              {
                points[i].w += point.w;
                map[4*by + bx] = i;
                match = MagickTrue;
                break;
              }
          }

          if (match != MagickFalse)
            continue;

          points[count].x = point.x;
          points[count].y = point.y;
          points[count].z = point.z;
          points[count].w = point.w;
          map[4*by + bx] = count;
          count++;
        }
      }

      for (i=0; i < (ssize_t) count; i++)
        points[i].w = sqrt(points[i].w);

      if (compression == FOURCC_DXT5)
        WriteAlphas(image,alphas,min5,max5,min7,max7);

      if (count == 1)
        WriteSingleColorFit(image,points,map);
      else
        WriteCompressed(image,count,points,map,clusterFit);
    }
  }
}
