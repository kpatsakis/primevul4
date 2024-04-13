MagickExport DrawInfo *CloneDrawInfo(const ImageInfo *image_info,
  const DrawInfo *draw_info)
{
  DrawInfo
    *clone_info;

  ExceptionInfo
    *exception;

  clone_info=(DrawInfo *) AcquireMagickMemory(sizeof(*clone_info));
  if (clone_info == (DrawInfo *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  GetDrawInfo(image_info,clone_info);
  if (draw_info == (DrawInfo *) NULL)
    return(clone_info);
  exception=AcquireExceptionInfo();
  if (clone_info->primitive != (char *) NULL)
    (void) CloneString(&clone_info->primitive,draw_info->primitive);
  if (draw_info->geometry != (char *) NULL)
    (void) CloneString(&clone_info->geometry,draw_info->geometry);
  clone_info->viewbox=draw_info->viewbox;
  clone_info->affine=draw_info->affine;
  clone_info->gravity=draw_info->gravity;
  clone_info->fill=draw_info->fill;
  clone_info->stroke=draw_info->stroke;
  clone_info->stroke_width=draw_info->stroke_width;
  if (draw_info->fill_pattern != (Image *) NULL)
    clone_info->fill_pattern=CloneImage(draw_info->fill_pattern,0,0,MagickTrue,
      exception);
  if (draw_info->stroke_pattern != (Image *) NULL)
    clone_info->stroke_pattern=CloneImage(draw_info->stroke_pattern,0,0,
      MagickTrue,exception);
  clone_info->stroke_antialias=draw_info->stroke_antialias;
  clone_info->text_antialias=draw_info->text_antialias;
  clone_info->fill_rule=draw_info->fill_rule;
  clone_info->linecap=draw_info->linecap;
  clone_info->linejoin=draw_info->linejoin;
  clone_info->miterlimit=draw_info->miterlimit;
  clone_info->dash_offset=draw_info->dash_offset;
  clone_info->decorate=draw_info->decorate;
  clone_info->compose=draw_info->compose;
  if (draw_info->text != (char *) NULL)
    (void) CloneString(&clone_info->text,draw_info->text);
  if (draw_info->font != (char *) NULL)
    (void) CloneString(&clone_info->font,draw_info->font);
  if (draw_info->metrics != (char *) NULL)
    (void) CloneString(&clone_info->metrics,draw_info->metrics);
  if (draw_info->family != (char *) NULL)
    (void) CloneString(&clone_info->family,draw_info->family);
  clone_info->style=draw_info->style;
  clone_info->stretch=draw_info->stretch;
  clone_info->weight=draw_info->weight;
  if (draw_info->encoding != (char *) NULL)
    (void) CloneString(&clone_info->encoding,draw_info->encoding);
  clone_info->pointsize=draw_info->pointsize;
  clone_info->kerning=draw_info->kerning;
  clone_info->interline_spacing=draw_info->interline_spacing;
  clone_info->interword_spacing=draw_info->interword_spacing;
  clone_info->direction=draw_info->direction;
  if (draw_info->density != (char *) NULL)
    (void) CloneString(&clone_info->density,draw_info->density);
  clone_info->align=draw_info->align;
  clone_info->undercolor=draw_info->undercolor;
  clone_info->border_color=draw_info->border_color;
  if (draw_info->server_name != (char *) NULL)
    (void) CloneString(&clone_info->server_name,draw_info->server_name);
  if (draw_info->dash_pattern != (double *) NULL)
    {
      register ssize_t
        x;

      for (x=0; draw_info->dash_pattern[x] != 0.0; x++) ;
      clone_info->dash_pattern=(double *) AcquireQuantumMemory((size_t) x+1UL,
        sizeof(*clone_info->dash_pattern));
      if (clone_info->dash_pattern == (double *) NULL)
        ThrowFatalException(ResourceLimitFatalError,
          "UnableToAllocateDashPattern");
      (void) CopyMagickMemory(clone_info->dash_pattern,draw_info->dash_pattern,
        (size_t) (x+1)*sizeof(*clone_info->dash_pattern));
    }
  clone_info->gradient=draw_info->gradient;
  if (draw_info->gradient.stops != (StopInfo *) NULL)
    {
      size_t
        number_stops;

      number_stops=clone_info->gradient.number_stops;
      clone_info->gradient.stops=(StopInfo *) AcquireQuantumMemory((size_t)
        number_stops,sizeof(*clone_info->gradient.stops));
      if (clone_info->gradient.stops == (StopInfo *) NULL)
        ThrowFatalException(ResourceLimitFatalError,
          "UnableToAllocateDashPattern");
      (void) CopyMagickMemory(clone_info->gradient.stops,
        draw_info->gradient.stops,(size_t) number_stops*
        sizeof(*clone_info->gradient.stops));
    }
  if (draw_info->clip_mask != (char *) NULL)
    (void) CloneString(&clone_info->clip_mask,draw_info->clip_mask);
  clone_info->bounds=draw_info->bounds;
  clone_info->clip_units=draw_info->clip_units;
  clone_info->render=draw_info->render;
  clone_info->alpha=draw_info->alpha;
  clone_info->element_reference=draw_info->element_reference;
  clone_info->debug=IsEventLogging();
  exception=DestroyExceptionInfo(exception);
  return(clone_info);
}
