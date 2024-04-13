static int  correct_orientation(struct image_data *image, unsigned char **work_buff_ptr)
  {
  uint16 mirror, rotation;
  unsigned char *work_buff;

  work_buff = *work_buff_ptr;
  if ((image == NULL) || (work_buff == NULL))
    {
    TIFFError ("correct_orientatin", "Invalid image or buffer pointer");
    return (-1);
    }

  if ((image->adjustments & MIRROR_HORIZ) || (image->adjustments & MIRROR_VERT))
    {
    mirror = (uint16)(image->adjustments & MIRROR_BOTH);
    if (mirrorImage(image->spp, image->bps, mirror, 
        image->width, image->length, work_buff))
      {
      TIFFError ("correct_orientation", "Unable to mirror image");
      return (-1);
      }
    }

  if (image->adjustments & ROTATE_ANY)
    {
    if (image->adjustments & ROTATECW_90)
      rotation = (uint16) 90;
    else
    if (image->adjustments & ROTATECW_180)
      rotation = (uint16) 180;
    else
    if (image->adjustments & ROTATECW_270)
      rotation = (uint16) 270;
    else
      {
      TIFFError ("correct_orientation", "Invalid rotation value: %d", 
                  image->adjustments & ROTATE_ANY);
      return (-1);
      }
 
    if (rotateImage(rotation, image, &image->width, &image->length, work_buff_ptr))
      {
      TIFFError ("correct_orientation", "Unable to rotate image");
      return (-1);
      }
    image->orientation = ORIENTATION_TOPLEFT;
    }

  return (0);
  } /* end correct_orientation */
