void CLASS pre_interpolate()
{
  ushort (*img)[4];
  int row, col, c;

  if (shrink) {
    if (half_size) {
      height = iheight;
      width  = iwidth;
    } else {
      img = (ushort (*)[4]) calloc (height*width, sizeof *img);
      merror (img, "pre_interpolate()");
      for (row=0; row < height; row++)
	for (col=0; col < width; col++) {
	  c = fc(row,col);
	  img[row*width+col][c] = image[(row >> 1)*iwidth+(col >> 1)][c];
	}
      free (image);
      image = img;
      shrink = 0;
    }
  }
  if (filters && colors == 3) {
    if (four_color_rgb && colors++)
      mix_green = !half_size;
    else {
      for (row = FC(1,0) >> 1; row < height; row+=2)
	for (col = FC(row,1) & 1; col < width; col+=2)
	  image[row*width+col][1] = image[row*width+col][3];
      filters &= ~((filters & 0x55555555) << 1);
    }
  }
  if (half_size) filters = 0;
}
