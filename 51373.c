MagickExport MagickBooleanType CloneImageProperties(Image *image,
  const Image *clone_image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(clone_image != (const Image *) NULL);
  assert(clone_image->signature == MagickCoreSignature);
  if (clone_image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      clone_image->filename);
  (void) CopyMagickString(image->filename,clone_image->filename,
    MagickPathExtent);
  (void) CopyMagickString(image->magick_filename,clone_image->magick_filename,
    MagickPathExtent);
  image->compression=clone_image->compression;
  image->quality=clone_image->quality;
  image->depth=clone_image->depth;
  image->alpha_color=clone_image->alpha_color;
  image->background_color=clone_image->background_color;
  image->border_color=clone_image->border_color;
  image->transparent_color=clone_image->transparent_color;
  image->gamma=clone_image->gamma;
  image->chromaticity=clone_image->chromaticity;
  image->rendering_intent=clone_image->rendering_intent;
  image->black_point_compensation=clone_image->black_point_compensation;
  image->units=clone_image->units;
  image->montage=(char *) NULL;
  image->directory=(char *) NULL;
  (void) CloneString(&image->geometry,clone_image->geometry);
  image->offset=clone_image->offset;
  image->resolution.x=clone_image->resolution.x;
  image->resolution.y=clone_image->resolution.y;
  image->page=clone_image->page;
  image->tile_offset=clone_image->tile_offset;
  image->extract_info=clone_image->extract_info;
  image->filter=clone_image->filter;
  image->fuzz=clone_image->fuzz;
  image->intensity=clone_image->intensity;
  image->interlace=clone_image->interlace;
  image->interpolate=clone_image->interpolate;
  image->endian=clone_image->endian;
  image->gravity=clone_image->gravity;
  image->compose=clone_image->compose;
  image->orientation=clone_image->orientation;
  image->scene=clone_image->scene;
  image->dispose=clone_image->dispose;
  image->delay=clone_image->delay;
  image->ticks_per_second=clone_image->ticks_per_second;
  image->iterations=clone_image->iterations;
  image->total_colors=clone_image->total_colors;
  image->taint=clone_image->taint;
  image->progress_monitor=clone_image->progress_monitor;
  image->client_data=clone_image->client_data;
  image->start_loop=clone_image->start_loop;
  image->error=clone_image->error;
  image->signature=clone_image->signature;
  if (clone_image->properties != (void *) NULL)
    {
      if (image->properties != (void *) NULL)
        DestroyImageProperties(image);
      image->properties=CloneSplayTree((SplayTreeInfo *)
        clone_image->properties,(void *(*)(void *)) ConstantString,
        (void *(*)(void *)) ConstantString);
    }
  return(MagickTrue);
}
