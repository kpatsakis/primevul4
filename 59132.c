get_body_files (const char* filename,
		const char pref,
		const MessageBody* body)
{
    File **files = NULL;
    VarLenData **data;
    char *ext = "";
    char *type = "unknown";
    int i;

    switch (pref)
    {
    case 'r':
	data = body->rtf_bodies;
	ext = ".rtf";
            type = "text/rtf";
	break;
    case 'h':
	data = body->html_bodies;
	ext = ".html";
            type = "text/html";
	break;
    case 't':
	data = body->text_body;
	ext = ".txt";
            type = "text/plain";
	break;
    default:
	data = NULL;
	break;
    }

    if (data)
    {
	int count = 0;
	char *tmp 
	    = CHECKED_XCALLOC(char, 
			      strlen(filename) + strlen(ext) + 1);
	strcpy (tmp, filename);
	strcat (tmp, ext);

        char *mime = CHECKED_XCALLOC(char, strlen(type) + 1);
        strcpy (mime, type);

	/* first get a count */
	while (data[count++]);

	files = (File**)XCALLOC(File*, count + 1);
	for (i = 0; data[i]; i++)
	{
	    files[i] = (File*)XCALLOC(File, 1);
	    files[i]->name = tmp;
            files[i]->mime_type = mime;
	    files[i]->len = data[i]->len;
	    files[i]->data 
		= CHECKED_XMALLOC(unsigned char, data[i]->len);
	    memmove (files[i]->data, data[i]->data, data[i]->len);
	}
    }
    return files;
}
