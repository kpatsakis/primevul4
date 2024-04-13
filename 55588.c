static MagickBooleanType WriteTXTImage(const ImageInfo *image_info,Image *image)
{
  char
    buffer[MaxTextExtent],
    colorspace[MaxTextExtent],
    tuple[MaxTextExtent];

  MagickBooleanType
    status;

  MagickOffsetType
    scene;

  MagickPixelPacket
    pixel;

  register const IndexPacket
    *indexes;

  register const PixelPacket
    *p;

  register ssize_t
    x;

  ssize_t
    y;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  status=OpenBlob(image_info,image,WriteBlobMode,&image->exception);
  if (status == MagickFalse)
    return(status);
  scene=0;
  do
  {
    ComplianceType
      compliance;

    const char
      *value;

    (void) CopyMagickString(colorspace,CommandOptionToMnemonic(
      MagickColorspaceOptions,(ssize_t) image->colorspace),MaxTextExtent);
    LocaleLower(colorspace);
    image->depth=GetImageQuantumDepth(image,MagickTrue);
    if (image->matte != MagickFalse)
      (void) ConcatenateMagickString(colorspace,"a",MaxTextExtent);
    compliance=NoCompliance;
    value=GetImageOption(image_info,"txt:compliance");
    if (value != (char *) NULL)
      compliance=(ComplianceType) ParseCommandOption(MagickComplianceOptions,
        MagickFalse,value);
    if (LocaleCompare(image_info->magick,"SPARSE-COLOR") != 0)
      {
        size_t
          depth;

        depth=compliance == SVGCompliance ? image->depth :
          MAGICKCORE_QUANTUM_DEPTH;
        (void) FormatLocaleString(buffer,MaxTextExtent,
          "# ImageMagick pixel enumeration: %.20g,%.20g,%.20g,%s\n",(double)
          image->columns,(double) image->rows,(double) ((MagickOffsetType)
          GetQuantumRange(depth)),colorspace);
        (void) WriteBlobString(image,buffer);
      }
    GetMagickPixelPacket(image,&pixel);
    for (y=0; y < (ssize_t) image->rows; y++)
    {
      p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
      if (p == (const PixelPacket *) NULL)
        break;
      indexes=GetVirtualIndexQueue(image);
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        SetMagickPixelPacket(image,p,indexes+x,&pixel);
        if (pixel.colorspace == LabColorspace)
          {
            pixel.green-=(QuantumRange+1)/2.0;
            pixel.blue-=(QuantumRange+1)/2.0;
          }
        if (LocaleCompare(image_info->magick,"SPARSE-COLOR") == 0)
          {
            /*
              Sparse-color format.
            */
            if (GetPixelOpacity(p) == (Quantum) OpaqueOpacity)
              {
                GetColorTuple(&pixel,MagickFalse,tuple);
                (void) QueryMagickColorname(image,&pixel,SVGCompliance,tuple,
                  &image->exception);
                (void) FormatLocaleString(buffer,MaxTextExtent,"%.20g,%.20g,",
                  (double) x,(double) y);
                (void) WriteBlobString(image,buffer);
                (void) WriteBlobString(image,tuple);
                (void) WriteBlobString(image," ");
              }
            p++;
            continue;
          }
        (void) FormatLocaleString(buffer,MaxTextExtent,"%.20g,%.20g: ",(double)
          x,(double) y);
        (void) WriteBlobString(image,buffer);
        (void) CopyMagickString(tuple,"(",MaxTextExtent);
        ConcatenateColorComponent(&pixel,RedChannel,compliance,tuple);
        (void) ConcatenateMagickString(tuple,",",MaxTextExtent);
        ConcatenateColorComponent(&pixel,GreenChannel,compliance,tuple);
        (void) ConcatenateMagickString(tuple,",",MaxTextExtent);
        ConcatenateColorComponent(&pixel,BlueChannel,compliance,tuple);
        if (pixel.colorspace == CMYKColorspace)
          {
            (void) ConcatenateMagickString(tuple,",",MaxTextExtent);
            ConcatenateColorComponent(&pixel,IndexChannel,compliance,tuple);
          }
        if (pixel.matte != MagickFalse)
          {
            (void) ConcatenateMagickString(tuple,",",MaxTextExtent);
            ConcatenateColorComponent(&pixel,AlphaChannel,compliance,tuple);
          }
        (void) ConcatenateMagickString(tuple,")",MaxTextExtent);
        (void) WriteBlobString(image,tuple);
        (void) WriteBlobString(image,"  ");
        GetColorTuple(&pixel,MagickTrue,tuple);
        (void) FormatLocaleString(buffer,MaxTextExtent,"%s",tuple);
        (void) WriteBlobString(image,buffer);
        (void) WriteBlobString(image,"  ");
        (void) QueryMagickColorname(image,&pixel,SVGCompliance,tuple,
          &image->exception);
        (void) WriteBlobString(image,tuple);
        (void) WriteBlobString(image,"\n");
        p++;
      }
      status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
        image->rows);
      if (status == MagickFalse)
        break;
    }
    if (GetNextImageInList(image) == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=SetImageProgress(image,SaveImagesTag,scene++,
      GetImageListLength(image));
    if (status == MagickFalse)
      break;
  } while (image_info->adjoin != MagickFalse);
  (void) CloseBlob(image);
  return(MagickTrue);
}
