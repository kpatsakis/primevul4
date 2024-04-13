MagickExport DrawInfo *DestroyDrawInfo(DrawInfo *draw_info)
{
  if (draw_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickCoreSignature);
  if (draw_info->primitive != (char *) NULL)
    draw_info->primitive=DestroyString(draw_info->primitive);
  if (draw_info->text != (char *) NULL)
    draw_info->text=DestroyString(draw_info->text);
  if (draw_info->geometry != (char *) NULL)
    draw_info->geometry=DestroyString(draw_info->geometry);
  if (draw_info->fill_pattern != (Image *) NULL)
    draw_info->fill_pattern=DestroyImage(draw_info->fill_pattern);
  if (draw_info->stroke_pattern != (Image *) NULL)
    draw_info->stroke_pattern=DestroyImage(draw_info->stroke_pattern);
  if (draw_info->font != (char *) NULL)
    draw_info->font=DestroyString(draw_info->font);
  if (draw_info->metrics != (char *) NULL)
    draw_info->metrics=DestroyString(draw_info->metrics);
  if (draw_info->family != (char *) NULL)
    draw_info->family=DestroyString(draw_info->family);
  if (draw_info->encoding != (char *) NULL)
    draw_info->encoding=DestroyString(draw_info->encoding);
  if (draw_info->density != (char *) NULL)
    draw_info->density=DestroyString(draw_info->density);
  if (draw_info->server_name != (char *) NULL)
    draw_info->server_name=(char *)
     RelinquishMagickMemory(draw_info->server_name);
  if (draw_info->dash_pattern != (double *) NULL)
    draw_info->dash_pattern=(double *) RelinquishMagickMemory(
      draw_info->dash_pattern);
  if (draw_info->gradient.stops != (StopInfo *) NULL)
    draw_info->gradient.stops=(StopInfo *) RelinquishMagickMemory(
      draw_info->gradient.stops);
  if (draw_info->clip_mask != (char *) NULL)
    draw_info->clip_mask=DestroyString(draw_info->clip_mask);
  draw_info->signature=(~MagickCoreSignature);
  draw_info=(DrawInfo *) RelinquishMagickMemory(draw_info);
  return(draw_info);
}
