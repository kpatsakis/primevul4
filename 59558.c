static MagickBooleanType WritePTIFImage(const ImageInfo *image_info,
  Image *image)
{
  ExceptionInfo
    *exception;

  Image
    *images,
    *next,
    *pyramid_image;

  ImageInfo
    *write_info;

  MagickBooleanType
    status;

  PointInfo
    resolution;

  size_t
    columns,
    rows;

  /*
    Create pyramid-encoded TIFF image.
  */
  exception=(&image->exception);
  images=NewImageList();
  for (next=image; next != (Image *) NULL; next=GetNextImageInList(next))
  {
    Image
      *clone_image;

    clone_image=CloneImage(next,0,0,MagickFalse,exception);
    if (clone_image == (Image *) NULL)
      break;
    clone_image->previous=NewImageList();
    clone_image->next=NewImageList();
    (void) SetImageProperty(clone_image,"tiff:subfiletype","none");
    AppendImageToList(&images,clone_image);
    columns=next->columns;
    rows=next->rows;
    resolution.x=next->x_resolution;
    resolution.y=next->y_resolution;
    while ((columns > 64) && (rows > 64))
    {
      columns/=2;
      rows/=2;
      resolution.x/=2.0;
      resolution.y/=2.0;
      pyramid_image=ResizeImage(next,columns,rows,image->filter,image->blur,
        exception);
      if (pyramid_image == (Image *) NULL)
        break;
      pyramid_image->x_resolution=resolution.x;
      pyramid_image->y_resolution=resolution.y;
      (void) SetImageProperty(pyramid_image,"tiff:subfiletype","REDUCEDIMAGE");
      AppendImageToList(&images,pyramid_image);
    }
  }
  /*
    Write pyramid-encoded TIFF image.
  */
  write_info=CloneImageInfo(image_info);
  write_info->adjoin=MagickTrue;
  status=WriteTIFFImage(write_info,GetFirstImageInList(images));
  images=DestroyImageList(images);
  write_info=DestroyImageInfo(write_info);
  return(status);
}
