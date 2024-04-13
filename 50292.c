void  process_command_opts (int argc, char *argv[], char *mp, char *mode, uint32 *dirnum,
	                    uint16 *defconfig, uint16 *deffillorder, uint32 *deftilewidth,
                            uint32 *deftilelength, uint32 *defrowsperstrip,
		            struct crop_mask *crop_data, struct pagedef *page, 
                            struct dump_opts *dump,
                            unsigned int     *imagelist, unsigned int   *image_count )
    {
    int   c, good_args = 0;
    char *opt_offset   = NULL;    /* Position in string of value sought */
    char *opt_ptr      = NULL;    /* Pointer to next token in option set */
    char *sep          = NULL;    /* Pointer to a token separator */
    unsigned int  i, j, start, end;
#if !HAVE_DECL_OPTARG
    extern int   optind;
    extern char* optarg;
#endif

    *mp++ = 'w';
    *mp = '\0';
    while ((c = getopt(argc, argv,
       "ac:d:e:f:hil:m:p:r:stvw:z:BCD:E:F:H:I:J:K:LMN:O:P:R:S:U:V:X:Y:Z:")) != -1)
      {
    good_args++;
    switch (c) {
      case 'a': mode[0] = 'a';	/* append to output */
		break;
      case 'c':	if (!processCompressOptions(optarg)) /* compression scheme */
		  {
		  TIFFError ("Unknown compression option", "%s", optarg);
                  TIFFError ("For valid options type", "tiffcrop -h");
                  exit (-1);
                  }
		break;
      case 'd':	start = strtoul(optarg, NULL, 0); /* initial IFD offset */
	        if (start == 0)
                  {
		  TIFFError ("","Directory offset must be greater than zero");
		  TIFFError ("For valid options type", "tiffcrop -h");
                  exit (-1);
		  }
	        *dirnum = start - 1;
		break;
      case 'e': switch (tolower((int) optarg[0])) /* image export modes*/
                  {
		  case 'c': crop_data->exp_mode = ONE_FILE_COMPOSITE;
 		            crop_data->img_mode = COMPOSITE_IMAGES;
		            break; /* Composite */
		  case 'd': crop_data->exp_mode = ONE_FILE_SEPARATED;
 		            crop_data->img_mode = SEPARATED_IMAGES;
		            break; /* Divided */
		  case 'i': crop_data->exp_mode = FILE_PER_IMAGE_COMPOSITE;
 		            crop_data->img_mode = COMPOSITE_IMAGES;
		            break; /* Image */
		  case 'm': crop_data->exp_mode = FILE_PER_IMAGE_SEPARATED;
 		            crop_data->img_mode = SEPARATED_IMAGES;
		            break; /* Multiple */
		  case 's': crop_data->exp_mode = FILE_PER_SELECTION;
 		            crop_data->img_mode = SEPARATED_IMAGES;
		            break; /* Sections */
		  default:  TIFFError ("Unknown export mode","%s", optarg);
                            TIFFError ("For valid options type", "tiffcrop -h");
                            exit (-1);
                  }
	        break;
      case 'f':	if (streq(optarg, "lsb2msb"))	   /* fill order */
		  *deffillorder = FILLORDER_LSB2MSB;
		else if (streq(optarg, "msb2lsb"))
		  *deffillorder = FILLORDER_MSB2LSB;
		else
		  {
		  TIFFError ("Unknown fill order", "%s", optarg);
                  TIFFError ("For valid options type", "tiffcrop -h");
                  exit (-1);
                  }
		break;
      case 'h':	usage();
		break;
      case 'i':	ignore = TRUE;		/* ignore errors */
		break;
      case 'l':	outtiled = TRUE;	 /* tile length */
		*deftilelength = atoi(optarg);
		break;
      case 'p': /* planar configuration */
		if (streq(optarg, "separate"))
		  *defconfig = PLANARCONFIG_SEPARATE;
		else if (streq(optarg, "contig"))
		  *defconfig = PLANARCONFIG_CONTIG;
		else
		  {
		  TIFFError ("Unkown planar configuration", "%s", optarg);
                  TIFFError ("For valid options type", "tiffcrop -h");
                  exit (-1);
                  }
		break;
      case 'r':	/* rows/strip */
		*defrowsperstrip = atol(optarg);
		break;
      case 's':	/* generate stripped output */
		outtiled = FALSE;
		break;
      case 't':	/* generate tiled output */
		outtiled = TRUE;
		break;
      case 'v': TIFFError("Library Release", "%s", TIFFGetVersion());
                TIFFError ("Tiffcrop version", "%s, last updated: %s", 
			   tiffcrop_version_id, tiffcrop_rev_date);
 	        TIFFError ("Tiffcp code", "Copyright (c) 1988-1997 Sam Leffler");
		TIFFError ("           ", "Copyright (c) 1991-1997 Silicon Graphics, Inc");
                TIFFError ("Tiffcrop additions", "Copyright (c) 2007-2010 Richard Nolde");
	        exit (0);
		break;
      case 'w':	/* tile width */
		outtiled = TRUE;
		*deftilewidth = atoi(optarg);
		break;
      case 'z': /* regions of an image specified as x1,y1,x2,y2:x3,y3,x4,y4 etc */
	        crop_data->crop_mode |= CROP_REGIONS;
		for (i = 0, opt_ptr = strtok (optarg, ":");
                   ((opt_ptr != NULL) &&  (i < MAX_REGIONS));
                    (opt_ptr = strtok (NULL, ":")), i++)
                    {
		    crop_data->regions++;
                    if (sscanf(opt_ptr, "%lf,%lf,%lf,%lf",
			       &crop_data->corners[i].X1, &crop_data->corners[i].Y1,
			       &crop_data->corners[i].X2, &crop_data->corners[i].Y2) != 4)
                      {
                      TIFFError ("Unable to parse coordinates for region", "%d %s", i, optarg);
		      TIFFError ("For valid options type", "tiffcrop -h");
                      exit (-1);
		      }
                    }
                /*  check for remaining elements over MAX_REGIONS */
                if ((opt_ptr != NULL) && (i >= MAX_REGIONS))
                  {
		  TIFFError ("Region list exceeds limit of", "%d regions %s", MAX_REGIONS, optarg);
		  TIFFError ("For valid options type", "tiffcrop -h");
                  exit (-1);;
                  }
		break;
      /* options for file open modes */
      case 'B': *mp++ = 'b'; *mp = '\0';
		break;
      case 'L': *mp++ = 'l'; *mp = '\0';
		break;
      case 'M': *mp++ = 'm'; *mp = '\0';
		break;
      case 'C': *mp++ = 'c'; *mp = '\0';
		break;
      /* options for Debugging / data dump */
      case 'D': for (i = 0, opt_ptr = strtok (optarg, ",");
                    (opt_ptr != NULL);
                    (opt_ptr = strtok (NULL, ",")), i++)
                    {
		    opt_offset = strpbrk(opt_ptr, ":=");
                    if (opt_offset == NULL)
                      {
                      TIFFError("Invalid dump option", "%s", optarg);
                      TIFFError ("For valid options type", "tiffcrop -h");
                      exit (-1);
		      }
                      
                    *opt_offset = '\0';
                    /* convert option to lowercase */
                    end = strlen (opt_ptr);
                    for (i = 0; i < end; i++)
                      *(opt_ptr + i) = tolower((int) *(opt_ptr + i));
                    /* Look for dump format specification */
                    if (strncmp(opt_ptr, "for", 3) == 0)
                      {
		      /* convert value to lowercase */
                      end = strlen (opt_offset + 1);
                      for (i = 1; i <= end; i++)
                        *(opt_offset + i) = tolower((int) *(opt_offset + i));
                      /* check dump format value */
		      if (strncmp (opt_offset + 1, "txt", 3) == 0)
                        {
                        dump->format = DUMP_TEXT;
                        strcpy (dump->mode, "w");
                        }
                      else
                        {
		        if (strncmp(opt_offset + 1, "raw", 3) == 0)
                          {
                          dump->format = DUMP_RAW;
                          strcpy (dump->mode, "wb");
                          }
                        else
                          {
                          TIFFError("parse_command_opts", "Unknown dump format %s", opt_offset + 1);
                          TIFFError ("For valid options type", "tiffcrop -h");
                          exit (-1);
		          }
			}
                      }
		    else
                      { /* Look for dump level specification */
                      if (strncmp (opt_ptr, "lev", 3) == 0)
                        dump->level = atoi(opt_offset + 1);
                        /* Look for input data dump file name */
                      if (strncmp (opt_ptr, "in", 2) == 0)
		        {
                        strncpy (dump->infilename, opt_offset + 1, PATH_MAX - 20);
                        dump->infilename[PATH_MAX - 20] = '\0';
                        }
                        /* Look for output data dump file name */
                      if (strncmp (opt_ptr, "out", 3) == 0)
			{
                        strncpy (dump->outfilename, opt_offset + 1, PATH_MAX - 20);
                        dump->outfilename[PATH_MAX - 20] = '\0';
                        }
                      if (strncmp (opt_ptr, "deb", 3) == 0)
			dump->debug = atoi(opt_offset + 1);
		      }
                    }
	        if ((strlen(dump->infilename)) || (strlen(dump->outfilename)))
                  {
		  if (dump->level == 1)
                    TIFFError("","Defaulting to dump level 1, no data.");
	          if (dump->format == DUMP_NONE)
                    {
		    TIFFError("", "You must specify a dump format for dump files");
		    TIFFError ("For valid options type", "tiffcrop -h");
		    exit (-1);
		    }
                  }
	        break;

      /* image manipulation routine options */
      case 'm': /* margins to exclude from selection, uppercase M was already used */
		/* order of values must be TOP, LEFT, BOTTOM, RIGHT */
		crop_data->crop_mode |= CROP_MARGINS;
                for (i = 0, opt_ptr = strtok (optarg, ",:");
                    ((opt_ptr != NULL) &&  (i < 4));
                     (opt_ptr = strtok (NULL, ",:")), i++)
                    {
		    crop_data->margins[i] = atof(opt_ptr);
                    }
		break;
      case 'E':	/* edge reference */
		switch (tolower((int) optarg[0]))
                  {
		  case 't': crop_data->edge_ref = EDGE_TOP;
                            break;
                  case 'b': crop_data->edge_ref = EDGE_BOTTOM;
                             break;
                  case 'l': crop_data->edge_ref = EDGE_LEFT;
                            break;
                  case 'r': crop_data->edge_ref = EDGE_RIGHT;
                            break;
		  default:  TIFFError ("Edge reference must be top, bottom, left, or right", "%s", optarg);
			    TIFFError ("For valid options type", "tiffcrop -h");
                            exit (-1);
		  }
		break;
      case 'F': /* flip eg mirror image or cropped segment, M was already used */
		crop_data->crop_mode |= CROP_MIRROR;
		switch (tolower((int) optarg[0]))
                  {
		  case  'h': crop_data->mirror = MIRROR_HORIZ;
                             break;
                  case  'v': crop_data->mirror = MIRROR_VERT;
                             break;
                  case  'b': crop_data->mirror = MIRROR_BOTH;
                             break;
		  default:   TIFFError ("Flip mode must be horiz, vert, or both", "%s", optarg);
			     TIFFError ("For valid options type", "tiffcrop -h");
                             exit (-1);
		  }
		break;
      case 'H': /* set horizontal resolution to new value */
		page->hres = atof (optarg);
                page->mode |= PAGE_MODE_RESOLUTION;
		break;
      case 'I': /* invert the color space, eg black to white */
		crop_data->crop_mode |= CROP_INVERT;
                /* The PHOTOMETIC_INTERPRETATION tag may be updated */
                if (streq(optarg, "black"))
                  {
		  crop_data->photometric = PHOTOMETRIC_MINISBLACK;
		  continue;
                  }
                if (streq(optarg, "white"))
                  {
		  crop_data->photometric = PHOTOMETRIC_MINISWHITE;
                  continue;
                  }
                if (streq(optarg, "data")) 
                  {
		  crop_data->photometric = INVERT_DATA_ONLY;
                  continue;
                  }
                if (streq(optarg, "both"))
                  {
		  crop_data->photometric = INVERT_DATA_AND_TAG;
                  continue;
                  }

		TIFFError("Missing or unknown option for inverting PHOTOMETRIC_INTERPRETATION", "%s", optarg);
		TIFFError ("For valid options type", "tiffcrop -h");
                exit (-1);
		break;
      case 'J': /* horizontal margin for sectioned ouput pages */ 
		page->hmargin = atof(optarg);
                page->mode |= PAGE_MODE_MARGINS;
		break;
      case 'K': /* vertical margin for sectioned ouput pages*/ 
                page->vmargin = atof(optarg);
                page->mode |= PAGE_MODE_MARGINS;
		break;
      case 'N':	/* list of images to process */
                for (i = 0, opt_ptr = strtok (optarg, ",");
                    ((opt_ptr != NULL) &&  (i < MAX_IMAGES));
                     (opt_ptr = strtok (NULL, ",")))
                     { /* We do not know how many images are in file yet 
			* so we build a list to include the maximum allowed
                        * and follow it until we hit the end of the file.
                        * Image count is not accurate for odd, even, last
                        * so page numbers won't be valid either.
                        */
		     if (streq(opt_ptr, "odd"))
                       {
		       for (j = 1; j <= MAX_IMAGES; j += 2)
			 imagelist[i++] = j;
                       *image_count = (MAX_IMAGES - 1) / 2;
                       break;
		       }
		     else
                       {
		       if (streq(opt_ptr, "even"))
                         {
			 for (j = 2; j <= MAX_IMAGES; j += 2)
			   imagelist[i++] = j;
                         *image_count = MAX_IMAGES / 2;
                         break;
			 }
		       else
                         {
			 if (streq(opt_ptr, "last"))
			   imagelist[i++] = MAX_IMAGES;
			 else  /* single value between commas */
			   {
			   sep = strpbrk(opt_ptr, ":-");
			   if (!sep)
			     imagelist[i++] = atoi(opt_ptr);
                           else
                             {
			     *sep = '\0';
                             start = atoi (opt_ptr);
                             if (!strcmp((sep + 1), "last"))
			       end = MAX_IMAGES;
                             else
                               end = atoi (sep + 1);
                             for (j = start; j <= end && j - start + i < MAX_IMAGES; j++)
			       imagelist[i++] = j;
			     }
			   }
			 }
		      }
		    }
                *image_count = i;
		break;
      case 'O': /* page orientation */ 
		switch (tolower((int) optarg[0]))
                  {
		  case  'a': page->orient = ORIENTATION_AUTO;
                             break;
		  case  'p': page->orient = ORIENTATION_PORTRAIT;
                             break;
		  case  'l': page->orient = ORIENTATION_LANDSCAPE;
                             break;
		  default:  TIFFError ("Orientation must be portrait, landscape, or auto.", "%s", optarg);
			    TIFFError ("For valid options type", "tiffcrop -h");
                            exit (-1);
		  }
		break;
      case 'P': /* page size selection */ 
	        if (sscanf(optarg, "%lfx%lf", &page->width, &page->length) == 2)
                  {
                  strcpy (page->name, "Custom"); 
                  page->mode |= PAGE_MODE_PAPERSIZE;
                  break;
                  }
                if (get_page_geometry (optarg, page))
                  {
		  if (!strcmp(optarg, "list"))
                    {
		    TIFFError("", "Name            Width   Length (in inches)");
                    for (i = 0; i < MAX_PAPERNAMES - 1; i++)
                      TIFFError ("", "%-15.15s %5.2f   %5.2f", 
			       PaperTable[i].name, PaperTable[i].width, 
                               PaperTable[i].length);
		    exit (-1);                   
                    }
     
		  TIFFError ("Invalid paper size", "%s", optarg);
                  TIFFError ("", "Select one of:");
		  TIFFError("", "Name            Width   Length (in inches)");
                  for (i = 0; i < MAX_PAPERNAMES - 1; i++)
                    TIFFError ("", "%-15.15s %5.2f   %5.2f", 
			       PaperTable[i].name, PaperTable[i].width, 
                               PaperTable[i].length);
		  exit (-1);
		  }
		else
                  {
                  page->mode |= PAGE_MODE_PAPERSIZE;
		  }
		break;
      case 'R': /* rotate image or cropped segment */
		crop_data->crop_mode |= CROP_ROTATE;
		switch (strtoul(optarg, NULL, 0))
                  {
		  case  90:  crop_data->rotation = (uint16)90;
                             break;
                  case  180: crop_data->rotation = (uint16)180;
                             break;
                  case  270: crop_data->rotation = (uint16)270;
                             break;
		  default:   TIFFError ("Rotation must be 90, 180, or 270 degrees clockwise", "%s", optarg);
			     TIFFError ("For valid options type", "tiffcrop -h");
                             exit (-1);
		  }
		break;
      case 'S':	/* subdivide into Cols:Rows sections, eg 3:2 would be 3 across and 2 down */
		sep = strpbrk(optarg, ",:");
		if (sep)
                  {
                  *sep = '\0';
                  page->cols = atoi(optarg);
                  page->rows = atoi(sep +1);
		  }
                else
                  {
                  page->cols = atoi(optarg);
                  page->rows = atoi(optarg);
		  }
                if ((page->cols * page->rows) > MAX_SECTIONS)
                  {
		  TIFFError ("Limit for subdivisions, ie rows x columns, exceeded", "%d", MAX_SECTIONS);
		  exit (-1);
                  }
                page->mode |= PAGE_MODE_ROWSCOLS;
		break;
      case 'U':	/* units for measurements and offsets */
		if (streq(optarg, "in"))
                  {
		  crop_data->res_unit = RESUNIT_INCH;
		  page->res_unit = RESUNIT_INCH;
		  }
		else if (streq(optarg, "cm"))
		  {
		  crop_data->res_unit = RESUNIT_CENTIMETER;
		  page->res_unit = RESUNIT_CENTIMETER;
		  }
		else if (streq(optarg, "px"))
		  {
		  crop_data->res_unit = RESUNIT_NONE;
		  page->res_unit = RESUNIT_NONE;
		  }
		else
                  {
		  TIFFError ("Illegal unit of measure","%s", optarg);
		  TIFFError ("For valid options type", "tiffcrop -h");
                  exit (-1);
		  }
		break;
      case 'V': /* set vertical resolution to new value */
		page->vres = atof (optarg);
                page->mode |= PAGE_MODE_RESOLUTION;
		break;
      case 'X':	/* selection width */
		crop_data->crop_mode |= CROP_WIDTH;
		crop_data->width = atof(optarg);
		break;
      case 'Y':	/* selection length */
		crop_data->crop_mode |= CROP_LENGTH;
		crop_data->length = atof(optarg);
		break;
      case 'Z': /* zones of an image X:Y read as zone X of Y */
		crop_data->crop_mode |= CROP_ZONES;
		for (i = 0, opt_ptr = strtok (optarg, ",");
                   ((opt_ptr != NULL) &&  (i < MAX_REGIONS));
                    (opt_ptr = strtok (NULL, ",")), i++)
                    {
		    crop_data->zones++;
		    opt_offset = strchr(opt_ptr, ':');
		    if (!opt_offset) {
			TIFFError("Wrong parameter syntax for -Z", "tiffcrop -h");
			exit(-1);
		    }
                    *opt_offset = '\0';
                    crop_data->zonelist[i].position = atoi(opt_ptr);
                    crop_data->zonelist[i].total    = atoi(opt_offset + 1);
                    }
                /*  check for remaining elements over MAX_REGIONS */
                if ((opt_ptr != NULL) && (i >= MAX_REGIONS))
                  {
		  TIFFError("Zone list exceeds region limit", "%d",  MAX_REGIONS);
		  exit (-1);
                  }
		break;
    case '?':	TIFFError ("For valid options type", "tiffcrop -h");
                exit (-1);
		/*NOTREACHED*/
      }
    }
  }  /* end process_command_opts */
