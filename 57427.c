static ssize_t SmushYGap(const Image *smush_image,const Image *images,
  const ssize_t offset,ExceptionInfo *exception)
{
  CacheView
    *bottom_view,
    *top_view;

  const Image
    *bottom_image,
    *top_image;

  RectangleInfo
    bottom_geometry,
    top_geometry;

  register const PixelPacket
    *p;

  register ssize_t
    i,
    x;

  size_t
    gap;

  ssize_t
    y;

  if (images->previous == (Image *) NULL)
    return(0);
  bottom_image=images;
  SetGeometry(smush_image,&bottom_geometry);
  GravityAdjustGeometry(bottom_image->columns,bottom_image->rows,
    bottom_image->gravity,&bottom_geometry);
  top_image=images->previous;
  SetGeometry(smush_image,&top_geometry);
  GravityAdjustGeometry(top_image->columns,top_image->rows,top_image->gravity,
    &top_geometry);
  gap=bottom_image->rows;
  top_view=AcquireVirtualCacheView(top_image,exception);
  bottom_view=AcquireVirtualCacheView(bottom_image,exception);
  for (x=0; x < (ssize_t) smush_image->columns; x++)
  {
    for (y=(ssize_t) top_image->rows-1; y > 0; y--)
    {
      p=GetCacheViewVirtualPixels(top_view,top_geometry.x+x,y,1,1,exception);
      if ((p == (const PixelPacket *) NULL) ||
          (GetPixelOpacity(p) != TransparentOpacity) ||
          ((top_image->rows-y-1) >= gap))
        break;
    }
    i=(ssize_t) top_image->rows-y-1;
    for (y=0; y < (ssize_t) bottom_image->rows; y++)
    {
      p=GetCacheViewVirtualPixels(bottom_view,bottom_geometry.x+x,y,1,1,
        exception);
      if ((p == (const PixelPacket *) NULL) ||
          (GetPixelOpacity(p) != TransparentOpacity) ||
          ((y+i) >= (ssize_t) gap))
        break;
    }
    if ((y+i) < (ssize_t) gap)
      gap=(size_t) (y+i);
  }
  bottom_view=DestroyCacheView(bottom_view);
  top_view=DestroyCacheView(top_view);
  if (x < (ssize_t) smush_image->columns)
    return(offset);
  return((ssize_t) gap-offset);
}
