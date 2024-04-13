cifsFileInfo_get(struct cifsFileInfo *cifs_file)
{
	spin_lock(&cifs_file_list_lock);
	cifsFileInfo_get_locked(cifs_file);
	spin_unlock(&cifs_file_list_lock);
	return cifs_file;
}
