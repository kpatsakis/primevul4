static MagickBooleanType DrawPolygonPrimitive(Image *image,
  const DrawInfo *draw_info,const PrimitiveInfo *primitive_info,
  ExceptionInfo *exception)
{
  CacheView
    *image_view;

  MagickBooleanType
    fill,
    status;

  double
    mid;

  PolygonInfo
    **magick_restrict polygon_info;

  register EdgeInfo
    *p;

  register ssize_t
    i;

  SegmentInfo
    bounds;

  ssize_t
    start_y,
    stop_y,
    y;

  /*
    Compute bounding box.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickCoreSignature);
  assert(primitive_info != (PrimitiveInfo *) NULL);
  if (primitive_info->coordinates == 0)
    return(MagickTrue);
  polygon_info=AcquirePolygonThreadSet(primitive_info);
  if (polygon_info == (PolygonInfo **) NULL)
    return(MagickFalse);
DisableMSCWarning(4127)
  if (0)
    DrawBoundingRectangles(image,draw_info,polygon_info[0],exception);
RestoreMSCWarning
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(DrawEvent,GetMagickModule(),"    begin draw-polygon");
  fill=(primitive_info->method == FillToBorderMethod) ||
    (primitive_info->method == FloodfillMethod) ? MagickTrue : MagickFalse;
  mid=ExpandAffine(&draw_info->affine)*draw_info->stroke_width/2.0;
  bounds=polygon_info[0]->edges[0].bounds;
  for (i=1; i < (ssize_t) polygon_info[0]->number_edges; i++)
  {
    p=polygon_info[0]->edges+i;
    if (p->bounds.x1 < bounds.x1)
      bounds.x1=p->bounds.x1;
    if (p->bounds.y1 < bounds.y1)
      bounds.y1=p->bounds.y1;
    if (p->bounds.x2 > bounds.x2)
      bounds.x2=p->bounds.x2;
    if (p->bounds.y2 > bounds.y2)
      bounds.y2=p->bounds.y2;
  }
  bounds.x1-=(mid+1.0);
  bounds.x1=bounds.x1 < 0.0 ? 0.0 : (size_t) ceil(bounds.x1-0.5) >=
    image->columns ? (double) image->columns-1 : bounds.x1;
  bounds.y1-=(mid+1.0);
  bounds.y1=bounds.y1 < 0.0 ? 0.0 : (size_t) ceil(bounds.y1-0.5) >=
    image->rows ? (double) image->rows-1 : bounds.y1;
  bounds.x2+=(mid+1.0);
  bounds.x2=bounds.x2 < 0.0 ? 0.0 : (size_t) floor(bounds.x2+0.5) >=
    image->columns ? (double) image->columns-1 : bounds.x2;
  bounds.y2+=(mid+1.0);
  bounds.y2=bounds.y2 < 0.0 ? 0.0 : (size_t) floor(bounds.y2+0.5) >=
    image->rows ? (double) image->rows-1 : bounds.y2;
  status=MagickTrue;
  image_view=AcquireAuthenticCacheView(image,exception);
  if ((primitive_info->coordinates == 1) ||
      (polygon_info[0]->number_edges == 0))
    {
      /*
        Draw point.
      */
      start_y=(ssize_t) ceil(bounds.y1-0.5);
      stop_y=(ssize_t) floor(bounds.y2+0.5);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
      #pragma omp parallel for schedule(static,4) shared(status) \
        magick_threads(image,image,1,1)
#endif
      for (y=start_y; y <= stop_y; y++)
      {
        MagickBooleanType
          sync;

        PixelInfo
          pixel;

        register ssize_t
          x;

        register Quantum
          *magick_restrict q;

        ssize_t
          start_x,
          stop_x;

        if (status == MagickFalse)
          continue;
        start_x=(ssize_t) ceil(bounds.x1-0.5);
        stop_x=(ssize_t) floor(bounds.x2+0.5);
        x=start_x;
        q=GetCacheViewAuthenticPixels(image_view,x,y,(size_t) (stop_x-x+1),1,
          exception);
        if (q == (Quantum *) NULL)
          {
            status=MagickFalse;
            continue;
          }
        GetPixelInfo(image,&pixel);
        for ( ; x <= stop_x; x++)
        {
          if ((x == (ssize_t) ceil(primitive_info->point.x-0.5)) &&
              (y == (ssize_t) ceil(primitive_info->point.y-0.5)))
            {
              GetFillColor(draw_info,x-start_x,y-start_y,&pixel,exception);
              SetPixelViaPixelInfo(image,&pixel,q);
            }
          q+=GetPixelChannels(image);
        }
        sync=SyncCacheViewAuthenticPixels(image_view,exception);
        if (sync == MagickFalse)
          status=MagickFalse;
      }
      image_view=DestroyCacheView(image_view);
      polygon_info=DestroyPolygonThreadSet(polygon_info);
      if (image->debug != MagickFalse)
        (void) LogMagickEvent(DrawEvent,GetMagickModule(),
          "    end draw-polygon");
      return(status);
    }
  /*
    Draw polygon or line.
  */
  if (image->alpha_trait == UndefinedPixelTrait)
    (void) SetImageAlphaChannel(image,OpaqueAlphaChannel,exception);
  start_y=(ssize_t) ceil(bounds.y1-0.5);
  stop_y=(ssize_t) floor(bounds.y2+0.5);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static,4) shared(status) \
    magick_threads(image,image,1,1)
#endif
  for (y=start_y; y <= stop_y; y++)
  {
    const int
      id = GetOpenMPThreadId();

    double
      fill_alpha,
      stroke_alpha;

    PixelInfo
      fill_color,
      stroke_color;

    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    ssize_t
      start_x,
      stop_x;

    if (status == MagickFalse)
      continue;
    start_x=(ssize_t) ceil(bounds.x1-0.5);
    stop_x=(ssize_t) floor(bounds.x2+0.5);
    q=GetCacheViewAuthenticPixels(image_view,start_x,y,(size_t) (stop_x-start_x+1),1,
      exception);
    if (q == (Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=start_x; x <= stop_x; x++)
    {
      /*
        Fill and/or stroke.
      */
      fill_alpha=GetFillAlpha(polygon_info[id],mid,fill,draw_info->fill_rule,
        x,y,&stroke_alpha);
      if (draw_info->stroke_antialias == MagickFalse)
        {
          fill_alpha=fill_alpha > 0.25 ? 1.0 : 0.0;
          stroke_alpha=stroke_alpha > 0.25 ? 1.0 : 0.0;
        }
      GetFillColor(draw_info,x-start_x,y-start_y,&fill_color,exception);
      fill_alpha=fill_alpha*fill_color.alpha;
      CompositePixelOver(image,&fill_color,fill_alpha,q,(double)
        GetPixelAlpha(image,q),q);
      GetStrokeColor(draw_info,x-start_x,y-start_y,&stroke_color,exception);
      stroke_alpha=stroke_alpha*stroke_color.alpha;
      CompositePixelOver(image,&stroke_color,stroke_alpha,q,(double)
        GetPixelAlpha(image,q),q);
      q+=GetPixelChannels(image);
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
  }
  image_view=DestroyCacheView(image_view);
  polygon_info=DestroyPolygonThreadSet(polygon_info);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(DrawEvent,GetMagickModule(),"    end draw-polygon");
  return(status);
}
