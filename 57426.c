MagickExport Image *SmushImages(const Image *images,
  const MagickBooleanType stack,const ssize_t offset,ExceptionInfo *exception)
{
#define SmushImageTag  "Smush/Image"

  CacheView
    *smush_view;

  const Image
    *image;

  Image
    *smush_image;

  MagickBooleanType
    matte,
    proceed,
    status;

  MagickOffsetType
    n;

  RectangleInfo
    geometry;

  register const Image
    *next;

  size_t
    height,
    number_images,
    width;

  ssize_t
    x_offset,
    y_offset;

  /*
    Compute maximum area of smushed area.
  */
  assert(images != (Image *) NULL);
  assert(images->signature == MagickSignature);
  if (images->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",images->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=images;
  matte=image->matte;
  number_images=1;
  width=image->columns;
  height=image->rows;
  next=GetNextImageInList(image);
  for ( ; next != (Image *) NULL; next=GetNextImageInList(next))
  {
    if (next->matte != MagickFalse)
      matte=MagickTrue;
    number_images++;
    if (stack != MagickFalse)
      {
        if (next->columns > width)
          width=next->columns;
        height+=next->rows;
        if (next->previous != (Image *) NULL)
          height+=offset;
        continue;
      }
    width+=next->columns;
    if (next->previous != (Image *) NULL)
      width+=offset;
    if (next->rows > height)
      height=next->rows;
  }
  /*
    Smush images.
  */
  smush_image=CloneImage(image,width,height,MagickTrue,exception);
  if (smush_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(smush_image,DirectClass) == MagickFalse)
    {
      InheritException(exception,&smush_image->exception);
      smush_image=DestroyImage(smush_image);
      return((Image *) NULL);
    }
  smush_image->matte=matte;
  (void) SetImageBackgroundColor(smush_image);
  status=MagickTrue;
  x_offset=0;
  y_offset=0;
  smush_view=AcquireVirtualCacheView(smush_image,exception);
  for (n=0; n < (MagickOffsetType) number_images; n++)
  {
    SetGeometry(smush_image,&geometry);
    GravityAdjustGeometry(image->columns,image->rows,image->gravity,&geometry);
    if (stack != MagickFalse)
      {
        x_offset-=geometry.x;
        y_offset-=SmushYGap(smush_image,image,offset,exception);
      }
    else
      {
        x_offset-=SmushXGap(smush_image,image,offset,exception);
        y_offset-=geometry.y;
      }
    status=CompositeImage(smush_image,OverCompositeOp,image,x_offset,y_offset);
    proceed=SetImageProgress(image,SmushImageTag,n,number_images);
    if (proceed == MagickFalse)
      break;
    if (stack == MagickFalse)
      {
        x_offset+=(ssize_t) image->columns;
        y_offset=0;
      }
    else
      {
        x_offset=0;
        y_offset+=(ssize_t) image->rows;
      }
    image=GetNextImageInList(image);
  }
  if (stack == MagickFalse)
    smush_image->columns=(size_t) x_offset;
  else
    smush_image->rows=(size_t) y_offset;
  smush_view=DestroyCacheView(smush_view);
  if (status == MagickFalse)
    smush_image=DestroyImage(smush_image);
  return(smush_image);
}
