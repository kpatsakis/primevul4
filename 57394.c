MagickExport ImageInfo *CloneImageInfo(const ImageInfo *image_info)
{
  ImageInfo
    *clone_info;

  clone_info=AcquireImageInfo();
  if (image_info == (ImageInfo *) NULL)
    return(clone_info);
  clone_info->compression=image_info->compression;
  clone_info->temporary=image_info->temporary;
  clone_info->adjoin=image_info->adjoin;
  clone_info->antialias=image_info->antialias;
  clone_info->scene=image_info->scene;
  clone_info->number_scenes=image_info->number_scenes;
  clone_info->depth=image_info->depth;
  (void) CloneString(&clone_info->size,image_info->size);
  (void) CloneString(&clone_info->extract,image_info->extract);
  (void) CloneString(&clone_info->scenes,image_info->scenes);
  (void) CloneString(&clone_info->page,image_info->page);
  clone_info->interlace=image_info->interlace;
  clone_info->endian=image_info->endian;
  clone_info->units=image_info->units;
  clone_info->quality=image_info->quality;
  (void) CloneString(&clone_info->sampling_factor,image_info->sampling_factor);
  (void) CloneString(&clone_info->server_name,image_info->server_name);
  (void) CloneString(&clone_info->font,image_info->font);
  (void) CloneString(&clone_info->texture,image_info->texture);
  (void) CloneString(&clone_info->density,image_info->density);
  clone_info->pointsize=image_info->pointsize;
  clone_info->fuzz=image_info->fuzz;
  clone_info->pen=image_info->pen;
  clone_info->background_color=image_info->background_color;
  clone_info->border_color=image_info->border_color;
  clone_info->matte_color=image_info->matte_color;
  clone_info->transparent_color=image_info->transparent_color;
  clone_info->dither=image_info->dither;
  clone_info->monochrome=image_info->monochrome;
  clone_info->colors=image_info->colors;
  clone_info->colorspace=image_info->colorspace;
  clone_info->type=image_info->type;
  clone_info->orientation=image_info->orientation;
  clone_info->preview_type=image_info->preview_type;
  clone_info->group=image_info->group;
  clone_info->ping=image_info->ping;
  clone_info->verbose=image_info->verbose;
  (void) CloneString(&clone_info->view,image_info->view);
  (void) CloneString(&clone_info->authenticate,image_info->authenticate);
  (void) CloneImageOptions(clone_info,image_info);
  clone_info->progress_monitor=image_info->progress_monitor;
  clone_info->client_data=image_info->client_data;
  clone_info->cache=image_info->cache;
  if (image_info->cache != (void *) NULL)
    clone_info->cache=ReferencePixelCache(image_info->cache);
  if (image_info->profile != (void *) NULL)
    clone_info->profile=(void *) CloneStringInfo((StringInfo *)
      image_info->profile);
  SetImageInfoFile(clone_info,image_info->file);
  SetImageInfoBlob(clone_info,image_info->blob,image_info->length);
  clone_info->stream=image_info->stream;
  clone_info->virtual_pixel_method=image_info->virtual_pixel_method;
  (void) CopyMagickString(clone_info->magick,image_info->magick,MaxTextExtent);
  (void) CopyMagickString(clone_info->unique,image_info->unique,MaxTextExtent);
  (void) CopyMagickString(clone_info->zero,image_info->zero,MaxTextExtent);
  (void) CopyMagickString(clone_info->filename,image_info->filename,
    MaxTextExtent);
  clone_info->subimage=image_info->scene;  /* deprecated */
  clone_info->subrange=image_info->number_scenes;  /* deprecated */
  clone_info->channel=image_info->channel;
  clone_info->debug=IsEventLogging();
  clone_info->signature=image_info->signature;
  return(clone_info);
}
