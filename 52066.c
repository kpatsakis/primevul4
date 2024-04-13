char *xmlrpc_normalizeBuffer(const char *buf)
{
	char *newbuf;
	int i, len, j = 0;

	len = strlen(buf);
	newbuf = (char *)smalloc(sizeof(char) * len + 1);

	for (i = 0; i < len; i++)
	{
		switch (buf[i])
		{
			  /* ctrl char */
		  case 1:
			  break;
			  /* Bold ctrl char */
		  case 2:
			  break;
			  /* Color ctrl char */
		  case 3:
			  /* If the next character is a digit, its also removed */
			  if (isdigit((unsigned char)buf[i + 1]))
			  {
				  i++;

				  /* not the best way to remove colors
				   * which are two digit but no worse then
				   * how the Unreal does with +S - TSL
				   */
				  if (isdigit((unsigned char)buf[i + 1]))
				  {
					  i++;
				  }

				  /* Check for background color code
				   * and remove it as well
				   */
				  if (buf[i + 1] == ',')
				  {
					  i++;

					  if (isdigit((unsigned char)buf[i + 1]))
					  {
						  i++;
					  }
					  /* not the best way to remove colors
					   * which are two digit but no worse then
					   * how the Unreal does with +S - TSL
					   */
					  if (isdigit((unsigned char)buf[i + 1]))
					  {
						  i++;
					  }
				  }
			  }

			  break;
			  /* tabs char */
		  case 9:
			  break;
			  /* line feed char */
		  case 10:
			  break;
			  /* carrage returns char */
		  case 13:
			  break;
			  /* Reverse ctrl char */
		  case 22:
			  break;
			  /* Underline ctrl char */
		  case 31:
			  break;
			  /* A valid char gets copied into the new buffer */
		  default:
			  /* All valid <32 characters are handled above. */
			  if (buf[i] > 31)
			  {
				newbuf[j] = buf[i];
				j++;
			  }
		}
	}

	/* Terminate the string */
	newbuf[j] = 0;

	return (newbuf);
}
