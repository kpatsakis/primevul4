main(int argc, char* argv[])
  {

#if !HAVE_DECL_OPTARG
  extern int optind;
#endif
  uint16 defconfig = (uint16) -1;
  uint16 deffillorder = 0;
  uint32 deftilewidth = (uint32) 0;
  uint32 deftilelength = (uint32) 0;
  uint32 defrowsperstrip = (uint32) 0;
  uint32 dirnum = 0;

  TIFF *in = NULL;
  TIFF *out = NULL;
  char  mode[10];
  char *mp = mode;

  /** RJN additions **/
  struct image_data image;     /* Image parameters for one image */
  struct crop_mask  crop;      /* Cropping parameters for all images */
  struct pagedef    page;      /* Page definition for output pages */
  struct pageseg    sections[MAX_SECTIONS];  /* Sections of one output page */
  struct buffinfo   seg_buffs[MAX_SECTIONS]; /* Segment buffer sizes and pointers */
  struct dump_opts  dump;                  /* Data dump options */
  unsigned char *read_buff    = NULL;      /* Input image data buffer */
  unsigned char *crop_buff    = NULL;      /* Crop area buffer */
  unsigned char *sect_buff    = NULL;      /* Image section buffer */
  unsigned char *sect_src     = NULL;      /* Image section buffer pointer */
  unsigned int  imagelist[MAX_IMAGES + 1]; /* individually specified images */
  unsigned int  image_count  = 0;
  unsigned int  dump_images  = 0;
  unsigned int  next_image   = 0;
  unsigned int  next_page    = 0;
  unsigned int  total_pages  = 0;
  unsigned int  total_images = 0;
  unsigned int  end_of_input = FALSE;
  int    seg, length;
  char   temp_filename[PATH_MAX + 1];

  little_endian = *((unsigned char *)&little_endian) & '1';

  initImageData(&image);
  initCropMasks(&crop);
  initPageSetup(&page, sections, seg_buffs);
  initDumpOptions(&dump);

  process_command_opts (argc, argv, mp, mode, &dirnum, &defconfig, 
                        &deffillorder, &deftilewidth, &deftilelength, &defrowsperstrip,
	                &crop, &page, &dump, imagelist, &image_count);

  if (argc - optind < 2)
    usage();

  if ((argc - optind) == 2)
    pageNum = -1;
  else
    total_images = 0;
  /* read multiple input files and write to output file(s) */
  while (optind < argc - 1)
    {
    in = TIFFOpen (argv[optind], "r");
    if (in == NULL)
      return (-3);

    /* If only one input file is specified, we can use directory count */
    total_images = TIFFNumberOfDirectories(in); 
    if (image_count == 0)
      {
      dirnum = 0;
      total_pages = total_images; /* Only valid with single input file */
      }
    else
      {
      dirnum = (tdir_t)(imagelist[next_image] - 1);
      next_image++;

      /* Total pages only valid for enumerated list of pages not derived
       * using odd, even, or last keywords.
       */
      if (image_count >  total_images)
	image_count = total_images;
      
      total_pages = image_count;
      }

    /* MAX_IMAGES is used for special case "last" in selection list */
    if (dirnum == (MAX_IMAGES - 1))
      dirnum = total_images - 1;

    if (dirnum > (total_images))
      {
      TIFFError (TIFFFileName(in), 
      "Invalid image number %d, File contains only %d images", 
		 (int)dirnum + 1, total_images);
      if (out != NULL)
        (void) TIFFClose(out);
      return (1);
      }

    if (dirnum != 0 && !TIFFSetDirectory(in, (tdir_t)dirnum))
      {
      TIFFError(TIFFFileName(in),"Error, setting subdirectory at %d", dirnum);
      if (out != NULL)
        (void) TIFFClose(out);
      return (1);
      }

    end_of_input = FALSE;
    while (end_of_input == FALSE)
      {
      config = defconfig;
      compression = defcompression;
      predictor = defpredictor;
      fillorder = deffillorder;
      rowsperstrip = defrowsperstrip;
      tilewidth = deftilewidth;
      tilelength = deftilelength;
      g3opts = defg3opts;

      if (dump.format != DUMP_NONE)
        {
        /* manage input and/or output dump files here */
	dump_images++;
        length = strlen(dump.infilename);
        if (length > 0)
          {
          if (dump.infile != NULL)
            fclose (dump.infile);

          /* dump.infilename is guaranteed to be NUL termimated and have 20 bytes 
             fewer than PATH_MAX */ 
          snprintf(temp_filename, sizeof(temp_filename), "%s-read-%03d.%s",
		   dump.infilename, dump_images,
                  (dump.format == DUMP_TEXT) ? "txt" : "raw");
          if ((dump.infile = fopen(temp_filename, dump.mode)) == NULL)
            {
	    TIFFError ("Unable to open dump file for writing", "%s", temp_filename);
	    exit (-1);
            }
          dump_info(dump.infile, dump.format, "Reading image","%d from %s", 
                    dump_images, TIFFFileName(in));
          } 
        length = strlen(dump.outfilename);
        if (length > 0)
          {
          if (dump.outfile != NULL)
            fclose (dump.outfile);

          /* dump.outfilename is guaranteed to be NUL termimated and have 20 bytes 
             fewer than PATH_MAX */ 
          snprintf(temp_filename, sizeof(temp_filename), "%s-write-%03d.%s",
		   dump.outfilename, dump_images,
                  (dump.format == DUMP_TEXT) ? "txt" : "raw");
          if ((dump.outfile = fopen(temp_filename, dump.mode)) == NULL)
            {
	      TIFFError ("Unable to open dump file for writing", "%s", temp_filename);
	    exit (-1);
            }
          dump_info(dump.outfile, dump.format, "Writing image","%d from %s", 
                    dump_images, TIFFFileName(in));
          } 
        }

      if (dump.debug)
         TIFFError("main", "Reading image %4d of %4d total pages.", dirnum + 1, total_pages);

      if (loadImage(in, &image, &dump, &read_buff))
        {
        TIFFError("main", "Unable to load source image");
        exit (-1);
        }

      /* Correct the image orientation if it was not ORIENTATION_TOPLEFT.
       */
      if (image.adjustments != 0)
        {
	if (correct_orientation(&image, &read_buff))
	    TIFFError("main", "Unable to correct image orientation");
        }

      if (getCropOffsets(&image, &crop, &dump))
        {
        TIFFError("main", "Unable to define crop regions");
        exit (-1);
	}

      if (crop.selections > 0)
        {
        if (processCropSelections(&image, &crop, &read_buff, seg_buffs))
          {
          TIFFError("main", "Unable to process image selections");
          exit (-1);
	  }
	}
      else  /* Single image segment without zones or regions */
        {
        if (createCroppedImage(&image, &crop, &read_buff, &crop_buff))
          {
          TIFFError("main", "Unable to create output image");
          exit (-1);
	  }
	}
      if (page.mode == PAGE_MODE_NONE)
        {  /* Whole image or sections not based on output page size */
        if (crop.selections > 0)
          {
	  writeSelections(in, &out, &crop, &image, &dump, seg_buffs,
                          mp, argv[argc - 1], &next_page, total_pages);
          }
	else  /* One file all images and sections */
          {
	  if (update_output_file (&out, mp, crop.exp_mode, argv[argc - 1],
                                  &next_page))
             exit (1);
          if (writeCroppedImage(in, out, &image, &dump,crop.combined_width, 
                                crop.combined_length, crop_buff, next_page, total_pages))
            {
             TIFFError("main", "Unable to write new image");
             exit (-1);
	    }
          }
	}
      else
        {
	/* If we used a crop buffer, our data is there, otherwise it is
         * in the read_buffer
         */
	if (crop_buff != NULL)  
	  sect_src = crop_buff;
        else
          sect_src = read_buff;
        /* Break input image into pages or rows and columns */
        if (computeOutputPixelOffsets(&crop, &image, &page, sections, &dump))
          {
          TIFFError("main", "Unable to compute output section data");
          exit (-1);
	  }
        /* If there are multiple files on the command line, the final one is assumed 
         * to be the output filename into which the images are written.
         */
	if (update_output_file (&out, mp, crop.exp_mode, argv[argc - 1], &next_page))
          exit (1);

	if (writeImageSections(in, out, &image, &page, sections, &dump, sect_src, &sect_buff))
          {
          TIFFError("main", "Unable to write image sections");
          exit (-1);
	  }
        }

      /* No image list specified, just read the next image */
      if (image_count == 0)
        dirnum++;
      else
        {
	dirnum = (tdir_t)(imagelist[next_image] - 1);
        next_image++;
        }

      if (dirnum == MAX_IMAGES - 1)
        dirnum = TIFFNumberOfDirectories(in) - 1;

      if (!TIFFSetDirectory(in, (tdir_t)dirnum))
        end_of_input = TRUE;
      }
    TIFFClose(in);
    optind++;
    }

  /* If we did not use the read buffer as the crop buffer */
  if (read_buff)
    _TIFFfree(read_buff);

  if (crop_buff)
    _TIFFfree(crop_buff);

  if (sect_buff)
    _TIFFfree(sect_buff);

   /* Clean up any segment buffers used for zones or regions */
  for (seg = 0; seg < crop.selections; seg++)
    _TIFFfree (seg_buffs[seg].buffer);

  if (dump.format != DUMP_NONE)
    {
    if (dump.infile != NULL)
     fclose (dump.infile);

    if (dump.outfile != NULL)
      {
      dump_info (dump.outfile, dump.format, "", "Completed run for %s", TIFFFileName(out));
      fclose (dump.outfile);
      }
    }

  TIFFClose(out);

  return (0);
  } /* end main */
