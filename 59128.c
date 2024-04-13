file_free (File *file)
{
    if (file)
    {
	XFREE (file->name);
	XFREE (file->data);
	XFREE (file->mime_type);
        XFREE (file->content_id);
        XFREE (file->path);
	memset (file, '\0', sizeof (File));
    }
}
