void feh_wm_set_bg(char *fil, Imlib_Image im, int centered, int scaled,
		int filled, int desktop, int use_filelist)
{
	XGCValues gcvalues;
	XGCValues gcval;
	GC gc;
	char bgname[20];
	int num = (int) rand();
	char bgfil[4096];
	char sendbuf[4096];

	/*
	 * TODO this re-implements mkstemp (badly). However, it is only needed
	 * for non-file images and enlightenment. Might be easier to just remove
	 * it.
	 */

	snprintf(bgname, sizeof(bgname), "FEHBG_%d", num);

	if (!fil && im) {
		if (getenv("HOME") == NULL) {
			weprintf("Cannot save wallpaper to temporary file: You have no HOME");
			return;
		}
		snprintf(bgfil, sizeof(bgfil), "%s/.%s.png", getenv("HOME"), bgname);
		imlib_context_set_image(im);
		imlib_image_set_format("png");
		gib_imlib_save_image(im, bgfil);
		D(("bg saved as %s\n", bgfil));
		fil = bgfil;
	}

	if (feh_wm_get_wm_is_e() && (enl_ipc_get_win() != None)) {
		if (use_filelist) {
			feh_wm_load_next(&im);
			fil = FEH_FILE(filelist->data)->filename;
		}
		snprintf(sendbuf, sizeof(sendbuf), "background %s bg.file %s", bgname, fil);
		enl_ipc_send(sendbuf);

		if (scaled) {
			snprintf(sendbuf, sizeof(sendbuf), "background %s bg.solid 0 0 0", bgname);
			enl_ipc_send(sendbuf);
			snprintf(sendbuf, sizeof(sendbuf), "background %s bg.tile 0", bgname);
			enl_ipc_send(sendbuf);
			snprintf(sendbuf, sizeof(sendbuf), "background %s bg.xjust 512", bgname);
			enl_ipc_send(sendbuf);
			snprintf(sendbuf, sizeof(sendbuf), "background %s bg.yjust 512", bgname);
			enl_ipc_send(sendbuf);
			snprintf(sendbuf, sizeof(sendbuf), "background %s bg.xperc 1024", bgname);
			enl_ipc_send(sendbuf);
			snprintf(sendbuf, sizeof(sendbuf), "background %s bg.yperc 1024", bgname);
			enl_ipc_send(sendbuf);
		} else if (centered) {
			snprintf(sendbuf, sizeof(sendbuf), "background %s bg.solid 0 0 0", bgname);
			enl_ipc_send(sendbuf);
			snprintf(sendbuf, sizeof(sendbuf), "background %s bg.tile 0", bgname);
			enl_ipc_send(sendbuf);
			snprintf(sendbuf, sizeof(sendbuf), "background %s bg.xjust 512", bgname);
			enl_ipc_send(sendbuf);
			snprintf(sendbuf, sizeof(sendbuf), "background %s bg.yjust 512", bgname);
			enl_ipc_send(sendbuf);
		} else {
			snprintf(sendbuf, sizeof(sendbuf), "background %s bg.tile 1", bgname);
			enl_ipc_send(sendbuf);
		}

		snprintf(sendbuf, sizeof(sendbuf), "use_bg %s %d", bgname, desktop);
		enl_ipc_send(sendbuf);
		enl_ipc_sync();
	} else {
		Atom prop_root, prop_esetroot, type;
		int format, i;
		unsigned long length, after;
		unsigned char *data_root = NULL, *data_esetroot = NULL;
		Pixmap pmap_d1, pmap_d2;
		gib_list *l;

		/* string for sticking in ~/.fehbg */
		char *fehbg = NULL;
		char fehbg_args[512];
		fehbg_args[0] = '\0';
		char *home;
		char filbuf[4096];
		char *bgfill = NULL;
		bgfill = opt.image_bg == IMAGE_BG_WHITE ?  "--image-bg white" : "--image-bg black" ;

#ifdef HAVE_LIBXINERAMA
		if (opt.xinerama) {
			if (opt.xinerama_index >= 0) {
				snprintf(fehbg_args, sizeof(fehbg_args),
					"--xinerama-index %d", opt.xinerama_index);
			}
		}
		else
			snprintf(fehbg_args, sizeof(fehbg_args), "--no-xinerama");
#endif			/* HAVE_LIBXINERAMA */

		/* local display to set closedownmode on */
		Display *disp2;
		Window root2;
		int depth2;
		int in, out, w, h;

		D(("Falling back to XSetRootWindowPixmap\n"));

		/* Put the filename in filbuf between ' and escape ' in the filename */
		out = 0;

		if (fil && !use_filelist) {
			filbuf[out++] = '\'';

			fil = feh_absolute_path(fil);

			for (in = 0; fil[in] && out < 4092; in++) {

				if (fil[in] == '\'')
					filbuf[out++] = '\\';
				filbuf[out++] = fil[in];
			}
			filbuf[out++] = '\'';
			free(fil);

		} else {
			for (l = filelist; l && out < 4092; l = l->next) {
				filbuf[out++] = '\'';

				fil = feh_absolute_path(FEH_FILE(l->data)->filename);

				for (in = 0; fil[in] && out < 4092; in++) {

					if (fil[in] == '\'')
						filbuf[out++] = '\\';
					filbuf[out++] = fil[in];
				}
				filbuf[out++] = '\'';
				filbuf[out++] = ' ';
				free(fil);
			}
		}


		filbuf[out++] = 0;

		if (scaled) {
			pmap_d1 = XCreatePixmap(disp, root, scr->width, scr->height, depth);

#ifdef HAVE_LIBXINERAMA
			if (opt.xinerama_index >= 0) {
				if (opt.image_bg == IMAGE_BG_WHITE)
					gcval.foreground = WhitePixel(disp, DefaultScreen(disp));
				else
					gcval.foreground = BlackPixel(disp, DefaultScreen(disp));
				gc = XCreateGC(disp, root, GCForeground, &gcval);
				XFillRectangle(disp, pmap_d1, gc, 0, 0, scr->width, scr->height);
				XFreeGC(disp, gc);
			}

			if (opt.xinerama && xinerama_screens) {
				for (i = 0; i < num_xinerama_screens; i++) {
					if (opt.xinerama_index < 0 || opt.xinerama_index == i) {
						feh_wm_set_bg_scaled(pmap_d1, im, use_filelist,
							xinerama_screens[i].x_org, xinerama_screens[i].y_org,
							xinerama_screens[i].width, xinerama_screens[i].height);
					}
				}
			}
			else
#endif			/* HAVE_LIBXINERAMA */
				feh_wm_set_bg_scaled(pmap_d1, im, use_filelist,
					0, 0, scr->width, scr->height);
			fehbg = estrjoin(" ", "feh", fehbg_args, "--bg-scale", filbuf, NULL);
		} else if (centered) {

			D(("centering\n"));

			pmap_d1 = XCreatePixmap(disp, root, scr->width, scr->height, depth);
			if (opt.image_bg == IMAGE_BG_WHITE)
				gcval.foreground = WhitePixel(disp, DefaultScreen(disp));
			else
				gcval.foreground = BlackPixel(disp, DefaultScreen(disp));
			gc = XCreateGC(disp, root, GCForeground, &gcval);
			XFillRectangle(disp, pmap_d1, gc, 0, 0, scr->width, scr->height);

#ifdef HAVE_LIBXINERAMA
			if (opt.xinerama && xinerama_screens) {
				for (i = 0; i < num_xinerama_screens; i++) {
					if (opt.xinerama_index < 0 || opt.xinerama_index == i) {
						feh_wm_set_bg_centered(pmap_d1, im, use_filelist,
							xinerama_screens[i].x_org, xinerama_screens[i].y_org,
							xinerama_screens[i].width, xinerama_screens[i].height);
					}
				}
			}
			else
#endif				/* HAVE_LIBXINERAMA */
				feh_wm_set_bg_centered(pmap_d1, im, use_filelist,
					0, 0, scr->width, scr->height);

			XFreeGC(disp, gc);

			fehbg = estrjoin(" ", "feh", fehbg_args, bgfill, "--bg-center", filbuf, NULL);

		} else if (filled == 1) {

			pmap_d1 = XCreatePixmap(disp, root, scr->width, scr->height, depth);

#ifdef HAVE_LIBXINERAMA
			if (opt.xinerama_index >= 0) {
				if (opt.image_bg == IMAGE_BG_WHITE)
					gcval.foreground = WhitePixel(disp, DefaultScreen(disp));
				else
					gcval.foreground = BlackPixel(disp, DefaultScreen(disp));
				gc = XCreateGC(disp, root, GCForeground, &gcval);
				XFillRectangle(disp, pmap_d1, gc, 0, 0, scr->width, scr->height);
				XFreeGC(disp, gc);
			}

			if (opt.xinerama && xinerama_screens) {
				for (i = 0; i < num_xinerama_screens; i++) {
					if (opt.xinerama_index < 0 || opt.xinerama_index == i) {
						feh_wm_set_bg_filled(pmap_d1, im, use_filelist,
							xinerama_screens[i].x_org, xinerama_screens[i].y_org,
							xinerama_screens[i].width, xinerama_screens[i].height);
					}
				}
			}
			else
#endif				/* HAVE_LIBXINERAMA */
				feh_wm_set_bg_filled(pmap_d1, im, use_filelist
					, 0, 0, scr->width, scr->height);

			fehbg = estrjoin(" ", "feh", fehbg_args, "--bg-fill", filbuf, NULL);

		} else if (filled == 2) {

			pmap_d1 = XCreatePixmap(disp, root, scr->width, scr->height, depth);
			if (opt.image_bg == IMAGE_BG_WHITE)
				gcval.foreground = WhitePixel(disp, DefaultScreen(disp));
			else
				gcval.foreground = BlackPixel(disp, DefaultScreen(disp));
			gc = XCreateGC(disp, root, GCForeground, &gcval);
			XFillRectangle(disp, pmap_d1, gc, 0, 0, scr->width, scr->height);

#ifdef HAVE_LIBXINERAMA
			if (opt.xinerama && xinerama_screens) {
				for (i = 0; i < num_xinerama_screens; i++) {
					if (opt.xinerama_index < 0 || opt.xinerama_index == i) {
						feh_wm_set_bg_maxed(pmap_d1, im, use_filelist,
							xinerama_screens[i].x_org, xinerama_screens[i].y_org,
							xinerama_screens[i].width, xinerama_screens[i].height);
					}
				}
			}
			else
#endif				/* HAVE_LIBXINERAMA */
				feh_wm_set_bg_maxed(pmap_d1, im, use_filelist,
					0, 0, scr->width, scr->height);

			XFreeGC(disp, gc);

			fehbg = estrjoin(" ", "feh", fehbg_args, bgfill, "--bg-max", filbuf, NULL);

		} else {
			if (use_filelist)
				feh_wm_load_next(&im);
			w = gib_imlib_image_get_width(im);
			h = gib_imlib_image_get_height(im);
			pmap_d1 = XCreatePixmap(disp, root, w, h, depth);
			gib_imlib_render_image_on_drawable(pmap_d1, im, 0, 0, 1, 0, 0);
			fehbg = estrjoin(" ", "feh --bg-tile", filbuf, NULL);
		}

		if (fehbg && !opt.no_fehbg) {
			home = getenv("HOME");
			if (home) {
				FILE *fp;
				char *path;
				struct stat s;
				path = estrjoin("/", home, ".fehbg", NULL);
				if ((fp = fopen(path, "w")) == NULL) {
					weprintf("Can't write to %s", path);
				} else {
					fprintf(fp, "#!/bin/sh\n%s\n", fehbg);
					fclose(fp);
					stat(path, &s);
					if (chmod(path, s.st_mode | S_IXUSR | S_IXGRP) != 0) {
						weprintf("Can't set %s as executable", path);
					}
				}
				free(path);
			}
		}
		
		free(fehbg);

		/* create new display, copy pixmap to new display */
		disp2 = XOpenDisplay(NULL);
		if (!disp2)
			eprintf("Can't reopen X display.");
		root2 = RootWindow(disp2, DefaultScreen(disp2));
		depth2 = DefaultDepth(disp2, DefaultScreen(disp2));
		XSync(disp, False);
		pmap_d2 = XCreatePixmap(disp2, root2, scr->width, scr->height, depth2);
		gcvalues.fill_style = FillTiled;
		gcvalues.tile = pmap_d1;
		gc = XCreateGC(disp2, pmap_d2, GCFillStyle | GCTile, &gcvalues);
		XFillRectangle(disp2, pmap_d2, gc, 0, 0, scr->width, scr->height);
		XFreeGC(disp2, gc);
		XSync(disp2, False);
		XSync(disp, False);
		XFreePixmap(disp, pmap_d1);

		prop_root = XInternAtom(disp2, "_XROOTPMAP_ID", True);
		prop_esetroot = XInternAtom(disp2, "ESETROOT_PMAP_ID", True);

		if (prop_root != None && prop_esetroot != None) {
			XGetWindowProperty(disp2, root2, prop_root, 0L, 1L,
					   False, AnyPropertyType, &type, &format, &length, &after, &data_root);
			if (type == XA_PIXMAP) {
				XGetWindowProperty(disp2, root2,
						   prop_esetroot, 0L, 1L,
						   False, AnyPropertyType,
						   &type, &format, &length, &after, &data_esetroot);
				if (data_root && data_esetroot) {
					if (type == XA_PIXMAP && *((Pixmap *) data_root) == *((Pixmap *) data_esetroot)) {
						XKillClient(disp2, *((Pixmap *)
								     data_root));
					}
				}
			}
		}

		if (data_root)
			XFree(data_root);
	
		if (data_esetroot)
			XFree(data_esetroot);

		/* This will locate the property, creating it if it doesn't exist */
		prop_root = XInternAtom(disp2, "_XROOTPMAP_ID", False);
		prop_esetroot = XInternAtom(disp2, "ESETROOT_PMAP_ID", False);

		if (prop_root == None || prop_esetroot == None)
			eprintf("creation of pixmap property failed.");

		XChangeProperty(disp2, root2, prop_root, XA_PIXMAP, 32, PropModeReplace, (unsigned char *) &pmap_d2, 1);
		XChangeProperty(disp2, root2, prop_esetroot, XA_PIXMAP, 32,
				PropModeReplace, (unsigned char *) &pmap_d2, 1);

		XSetWindowBackgroundPixmap(disp2, root2, pmap_d2);
		XClearWindow(disp2, root2);
		XFlush(disp2);
		XSetCloseDownMode(disp2, RetainPermanent);
		XCloseDisplay(disp2);
	}
	return;
}
