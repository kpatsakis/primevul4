cifs_parse_mount_options(char *options, const char *devname,
			 struct smb_vol *vol)
{
	char *value;
	char *data;
	unsigned int  temp_len, i, j;
	char separator[2];
	short int override_uid = -1;
	short int override_gid = -1;
	bool uid_specified = false;
	bool gid_specified = false;

	separator[0] = ',';
	separator[1] = 0;

	if (Local_System_Name[0] != 0)
		memcpy(vol->source_rfc1001_name, Local_System_Name, 15);
	else {
		char *nodename = utsname()->nodename;
		int n = strnlen(nodename, 15);
		memset(vol->source_rfc1001_name, 0x20, 15);
		for (i = 0; i < n; i++) {
			/* does not have to be perfect mapping since field is
			informational, only used for servers that do not support
			port 445 and it can be overridden at mount time */
			vol->source_rfc1001_name[i] = toupper(nodename[i]);
		}
	}
	vol->source_rfc1001_name[15] = 0;
	/* null target name indicates to use *SMBSERVR default called name
	   if we end up sending RFC1001 session initialize */
	vol->target_rfc1001_name[0] = 0;
	vol->linux_uid = current_uid();  /* use current_euid() instead? */
	vol->linux_gid = current_gid();

	/* default to only allowing write access to owner of the mount */
	vol->dir_mode = vol->file_mode = S_IRUGO | S_IXUGO | S_IWUSR;

	/* vol->retry default is 0 (i.e. "soft" limited retry not hard retry) */
	/* default is always to request posix paths. */
	vol->posix_paths = 1;
	/* default to using server inode numbers where available */
	vol->server_ino = 1;

	if (!options)
		return 1;

	if (strncmp(options, "sep=", 4) == 0) {
		if (options[4] != 0) {
			separator[0] = options[4];
			options += 5;
		} else {
			cFYI(1, "Null separator not allowed");
		}
	}

	while ((data = strsep(&options, separator)) != NULL) {
		if (!*data)
			continue;
		if ((value = strchr(data, '=')) != NULL)
			*value++ = '\0';

		/* Have to parse this before we parse for "user" */
		if (strnicmp(data, "user_xattr", 10) == 0) {
			vol->no_xattr = 0;
		} else if (strnicmp(data, "nouser_xattr", 12) == 0) {
			vol->no_xattr = 1;
		} else if (strnicmp(data, "user", 4) == 0) {
			if (!value) {
				printk(KERN_WARNING
				       "CIFS: invalid or missing username\n");
				return 1;	/* needs_arg; */
			} else if (!*value) {
				/* null user, ie anonymous, authentication */
				vol->nullauth = 1;
			}
			if (strnlen(value, 200) < 200) {
				vol->username = value;
			} else {
				printk(KERN_WARNING "CIFS: username too long\n");
				return 1;
			}
		} else if (strnicmp(data, "pass", 4) == 0) {
			if (!value) {
				vol->password = NULL;
				continue;
			} else if (value[0] == 0) {
				/* check if string begins with double comma
				   since that would mean the password really
				   does start with a comma, and would not
				   indicate an empty string */
				if (value[1] != separator[0]) {
					vol->password = NULL;
					continue;
				}
			}
			temp_len = strlen(value);
			/* removed password length check, NTLM passwords
				can be arbitrarily long */

			/* if comma in password, the string will be
			prematurely null terminated.  Commas in password are
			specified across the cifs mount interface by a double
			comma ie ,, and a comma used as in other cases ie ','
			as a parameter delimiter/separator is single and due
			to the strsep above is temporarily zeroed. */

			/* NB: password legally can have multiple commas and
			the only illegal character in a password is null */

			if ((value[temp_len] == 0) &&
			    (value[temp_len+1] == separator[0])) {
				/* reinsert comma */
				value[temp_len] = separator[0];
				temp_len += 2;  /* move after second comma */
				while (value[temp_len] != 0)  {
					if (value[temp_len] == separator[0]) {
						if (value[temp_len+1] ==
						     separator[0]) {
						/* skip second comma */
							temp_len++;
						} else {
						/* single comma indicating start
							 of next parm */
							break;
						}
					}
					temp_len++;
				}
				if (value[temp_len] == 0) {
					options = NULL;
				} else {
					value[temp_len] = 0;
					/* point option to start of next parm */
					options = value + temp_len + 1;
				}
				/* go from value to value + temp_len condensing
				double commas to singles. Note that this ends up
				allocating a few bytes too many, which is ok */
				vol->password = kzalloc(temp_len, GFP_KERNEL);
				if (vol->password == NULL) {
					printk(KERN_WARNING "CIFS: no memory "
							    "for password\n");
					return 1;
				}
				for (i = 0, j = 0; i < temp_len; i++, j++) {
					vol->password[j] = value[i];
					if (value[i] == separator[0]
						&& value[i+1] == separator[0]) {
						/* skip second comma */
						i++;
					}
				}
				vol->password[j] = 0;
			} else {
				vol->password = kzalloc(temp_len+1, GFP_KERNEL);
				if (vol->password == NULL) {
					printk(KERN_WARNING "CIFS: no memory "
							    "for password\n");
					return 1;
				}
				strcpy(vol->password, value);
			}
		} else if (!strnicmp(data, "ip", 2) ||
			   !strnicmp(data, "addr", 4)) {
			if (!value || !*value) {
				vol->UNCip = NULL;
			} else if (strnlen(value, INET6_ADDRSTRLEN) <
							INET6_ADDRSTRLEN) {
				vol->UNCip = value;
			} else {
				printk(KERN_WARNING "CIFS: ip address "
						    "too long\n");
				return 1;
			}
		} else if (strnicmp(data, "sec", 3) == 0) {
			if (!value || !*value) {
				cERROR(1, "no security value specified");
				continue;
			} else if (strnicmp(value, "krb5i", 5) == 0) {
				vol->secFlg |= CIFSSEC_MAY_KRB5 |
					CIFSSEC_MUST_SIGN;
			} else if (strnicmp(value, "krb5p", 5) == 0) {
				/* vol->secFlg |= CIFSSEC_MUST_SEAL |
					CIFSSEC_MAY_KRB5; */
				cERROR(1, "Krb5 cifs privacy not supported");
				return 1;
			} else if (strnicmp(value, "krb5", 4) == 0) {
				vol->secFlg |= CIFSSEC_MAY_KRB5;
#ifdef CONFIG_CIFS_EXPERIMENTAL
			} else if (strnicmp(value, "ntlmsspi", 8) == 0) {
				vol->secFlg |= CIFSSEC_MAY_NTLMSSP |
					CIFSSEC_MUST_SIGN;
			} else if (strnicmp(value, "ntlmssp", 7) == 0) {
				vol->secFlg |= CIFSSEC_MAY_NTLMSSP;
#endif
			} else if (strnicmp(value, "ntlmv2i", 7) == 0) {
				vol->secFlg |= CIFSSEC_MAY_NTLMV2 |
					CIFSSEC_MUST_SIGN;
			} else if (strnicmp(value, "ntlmv2", 6) == 0) {
				vol->secFlg |= CIFSSEC_MAY_NTLMV2;
			} else if (strnicmp(value, "ntlmi", 5) == 0) {
				vol->secFlg |= CIFSSEC_MAY_NTLM |
					CIFSSEC_MUST_SIGN;
			} else if (strnicmp(value, "ntlm", 4) == 0) {
				/* ntlm is default so can be turned off too */
				vol->secFlg |= CIFSSEC_MAY_NTLM;
			} else if (strnicmp(value, "nontlm", 6) == 0) {
				/* BB is there a better way to do this? */
				vol->secFlg |= CIFSSEC_MAY_NTLMV2;
#ifdef CONFIG_CIFS_WEAK_PW_HASH
			} else if (strnicmp(value, "lanman", 6) == 0) {
				vol->secFlg |= CIFSSEC_MAY_LANMAN;
#endif
			} else if (strnicmp(value, "none", 4) == 0) {
				vol->nullauth = 1;
			} else {
				cERROR(1, "bad security option: %s", value);
				return 1;
			}
		} else if ((strnicmp(data, "unc", 3) == 0)
			   || (strnicmp(data, "target", 6) == 0)
			   || (strnicmp(data, "path", 4) == 0)) {
			if (!value || !*value) {
				printk(KERN_WARNING "CIFS: invalid path to "
						    "network resource\n");
				return 1;	/* needs_arg; */
			}
			if ((temp_len = strnlen(value, 300)) < 300) {
				vol->UNC = kmalloc(temp_len+1, GFP_KERNEL);
				if (vol->UNC == NULL)
					return 1;
				strcpy(vol->UNC, value);
				if (strncmp(vol->UNC, "//", 2) == 0) {
					vol->UNC[0] = '\\';
					vol->UNC[1] = '\\';
				} else if (strncmp(vol->UNC, "\\\\", 2) != 0) {
					printk(KERN_WARNING
					       "CIFS: UNC Path does not begin "
					       "with // or \\\\ \n");
					return 1;
				}
			} else {
				printk(KERN_WARNING "CIFS: UNC name too long\n");
				return 1;
			}
		} else if ((strnicmp(data, "domain", 3) == 0)
			   || (strnicmp(data, "workgroup", 5) == 0)) {
			if (!value || !*value) {
				printk(KERN_WARNING "CIFS: invalid domain name\n");
				return 1;	/* needs_arg; */
			}
			/* BB are there cases in which a comma can be valid in
			a domain name and need special handling? */
			if (strnlen(value, 256) < 256) {
				vol->domainname = value;
				cFYI(1, "Domain name set");
			} else {
				printk(KERN_WARNING "CIFS: domain name too "
						    "long\n");
				return 1;
			}
		} else if (strnicmp(data, "prefixpath", 10) == 0) {
			if (!value || !*value) {
				printk(KERN_WARNING
					"CIFS: invalid path prefix\n");
				return 1;       /* needs_argument */
			}
			if ((temp_len = strnlen(value, 1024)) < 1024) {
				if (value[0] != '/')
					temp_len++;  /* missing leading slash */
				vol->prepath = kmalloc(temp_len+1, GFP_KERNEL);
				if (vol->prepath == NULL)
					return 1;
				if (value[0] != '/') {
					vol->prepath[0] = '/';
					strcpy(vol->prepath+1, value);
				} else
					strcpy(vol->prepath, value);
				cFYI(1, "prefix path %s", vol->prepath);
			} else {
				printk(KERN_WARNING "CIFS: prefix too long\n");
				return 1;
			}
		} else if (strnicmp(data, "iocharset", 9) == 0) {
			if (!value || !*value) {
				printk(KERN_WARNING "CIFS: invalid iocharset "
						    "specified\n");
				return 1;	/* needs_arg; */
			}
			if (strnlen(value, 65) < 65) {
				if (strnicmp(value, "default", 7))
					vol->iocharset = value;
				/* if iocharset not set then load_nls_default
				   is used by caller */
				cFYI(1, "iocharset set to %s", value);
			} else {
				printk(KERN_WARNING "CIFS: iocharset name "
						    "too long.\n");
				return 1;
			}
		} else if (!strnicmp(data, "uid", 3) && value && *value) {
			vol->linux_uid = simple_strtoul(value, &value, 0);
			uid_specified = true;
		} else if (!strnicmp(data, "forceuid", 8)) {
			override_uid = 1;
		} else if (!strnicmp(data, "noforceuid", 10)) {
			override_uid = 0;
		} else if (!strnicmp(data, "gid", 3) && value && *value) {
			vol->linux_gid = simple_strtoul(value, &value, 0);
			gid_specified = true;
		} else if (!strnicmp(data, "forcegid", 8)) {
			override_gid = 1;
		} else if (!strnicmp(data, "noforcegid", 10)) {
			override_gid = 0;
		} else if (strnicmp(data, "file_mode", 4) == 0) {
			if (value && *value) {
				vol->file_mode =
					simple_strtoul(value, &value, 0);
			}
		} else if (strnicmp(data, "dir_mode", 4) == 0) {
			if (value && *value) {
				vol->dir_mode =
					simple_strtoul(value, &value, 0);
			}
		} else if (strnicmp(data, "dirmode", 4) == 0) {
			if (value && *value) {
				vol->dir_mode =
					simple_strtoul(value, &value, 0);
			}
		} else if (strnicmp(data, "port", 4) == 0) {
			if (value && *value) {
				vol->port =
					simple_strtoul(value, &value, 0);
			}
		} else if (strnicmp(data, "rsize", 5) == 0) {
			if (value && *value) {
				vol->rsize =
					simple_strtoul(value, &value, 0);
			}
		} else if (strnicmp(data, "wsize", 5) == 0) {
			if (value && *value) {
				vol->wsize =
					simple_strtoul(value, &value, 0);
			}
		} else if (strnicmp(data, "sockopt", 5) == 0) {
			if (!value || !*value) {
				cERROR(1, "no socket option specified");
				continue;
			} else if (strnicmp(value, "TCP_NODELAY", 11) == 0) {
				vol->sockopt_tcp_nodelay = 1;
			}
		} else if (strnicmp(data, "netbiosname", 4) == 0) {
			if (!value || !*value || (*value == ' ')) {
				cFYI(1, "invalid (empty) netbiosname");
			} else {
				memset(vol->source_rfc1001_name, 0x20, 15);
				for (i = 0; i < 15; i++) {
				/* BB are there cases in which a comma can be
				valid in this workstation netbios name (and need
				special handling)? */

				/* We do not uppercase netbiosname for user */
					if (value[i] == 0)
						break;
					else
						vol->source_rfc1001_name[i] =
								value[i];
				}
				/* The string has 16th byte zero still from
				set at top of the function  */
				if ((i == 15) && (value[i] != 0))
					printk(KERN_WARNING "CIFS: netbiosname"
						" longer than 15 truncated.\n");
			}
		} else if (strnicmp(data, "servern", 7) == 0) {
			/* servernetbiosname specified override *SMBSERVER */
			if (!value || !*value || (*value == ' ')) {
				cFYI(1, "empty server netbiosname specified");
			} else {
				/* last byte, type, is 0x20 for servr type */
				memset(vol->target_rfc1001_name, 0x20, 16);

				for (i = 0; i < 15; i++) {
				/* BB are there cases in which a comma can be
				   valid in this workstation netbios name
				   (and need special handling)? */

				/* user or mount helper must uppercase
				   the netbiosname */
					if (value[i] == 0)
						break;
					else
						vol->target_rfc1001_name[i] =
								value[i];
				}
				/* The string has 16th byte zero still from
				   set at top of the function  */
				if ((i == 15) && (value[i] != 0))
					printk(KERN_WARNING "CIFS: server net"
					"biosname longer than 15 truncated.\n");
			}
		} else if (strnicmp(data, "credentials", 4) == 0) {
			/* ignore */
		} else if (strnicmp(data, "version", 3) == 0) {
			/* ignore */
		} else if (strnicmp(data, "guest", 5) == 0) {
			/* ignore */
		} else if (strnicmp(data, "rw", 2) == 0) {
			/* ignore */
		} else if (strnicmp(data, "ro", 2) == 0) {
			/* ignore */
		} else if (strnicmp(data, "noblocksend", 11) == 0) {
			vol->noblocksnd = 1;
		} else if (strnicmp(data, "noautotune", 10) == 0) {
			vol->noautotune = 1;
		} else if ((strnicmp(data, "suid", 4) == 0) ||
				   (strnicmp(data, "nosuid", 6) == 0) ||
				   (strnicmp(data, "exec", 4) == 0) ||
				   (strnicmp(data, "noexec", 6) == 0) ||
				   (strnicmp(data, "nodev", 5) == 0) ||
				   (strnicmp(data, "noauto", 6) == 0) ||
				   (strnicmp(data, "dev", 3) == 0)) {
			/*  The mount tool or mount.cifs helper (if present)
			    uses these opts to set flags, and the flags are read
			    by the kernel vfs layer before we get here (ie
			    before read super) so there is no point trying to
			    parse these options again and set anything and it
			    is ok to just ignore them */
			continue;
		} else if (strnicmp(data, "hard", 4) == 0) {
			vol->retry = 1;
		} else if (strnicmp(data, "soft", 4) == 0) {
			vol->retry = 0;
		} else if (strnicmp(data, "perm", 4) == 0) {
			vol->noperm = 0;
		} else if (strnicmp(data, "noperm", 6) == 0) {
			vol->noperm = 1;
		} else if (strnicmp(data, "mapchars", 8) == 0) {
			vol->remap = 1;
		} else if (strnicmp(data, "nomapchars", 10) == 0) {
			vol->remap = 0;
		} else if (strnicmp(data, "sfu", 3) == 0) {
			vol->sfu_emul = 1;
		} else if (strnicmp(data, "nosfu", 5) == 0) {
			vol->sfu_emul = 0;
		} else if (strnicmp(data, "nodfs", 5) == 0) {
			vol->nodfs = 1;
		} else if (strnicmp(data, "posixpaths", 10) == 0) {
			vol->posix_paths = 1;
		} else if (strnicmp(data, "noposixpaths", 12) == 0) {
			vol->posix_paths = 0;
		} else if (strnicmp(data, "nounix", 6) == 0) {
			vol->no_linux_ext = 1;
		} else if (strnicmp(data, "nolinux", 7) == 0) {
			vol->no_linux_ext = 1;
		} else if ((strnicmp(data, "nocase", 6) == 0) ||
			   (strnicmp(data, "ignorecase", 10)  == 0)) {
			vol->nocase = 1;
		} else if (strnicmp(data, "brl", 3) == 0) {
			vol->nobrl =  0;
		} else if ((strnicmp(data, "nobrl", 5) == 0) ||
			   (strnicmp(data, "nolock", 6) == 0)) {
			vol->nobrl =  1;
			/* turn off mandatory locking in mode
			if remote locking is turned off since the
			local vfs will do advisory */
			if (vol->file_mode ==
				(S_IALLUGO & ~(S_ISUID | S_IXGRP)))
				vol->file_mode = S_IALLUGO;
		} else if (strnicmp(data, "forcemandatorylock", 9) == 0) {
			/* will take the shorter form "forcemand" as well */
			/* This mount option will force use of mandatory
			  (DOS/Windows style) byte range locks, instead of
			  using posix advisory byte range locks, even if the
			  Unix extensions are available and posix locks would
			  be supported otherwise. If Unix extensions are not
			  negotiated this has no effect since mandatory locks
			  would be used (mandatory locks is all that those
			  those servers support) */
			vol->mand_lock = 1;
		} else if (strnicmp(data, "setuids", 7) == 0) {
			vol->setuids = 1;
		} else if (strnicmp(data, "nosetuids", 9) == 0) {
			vol->setuids = 0;
		} else if (strnicmp(data, "dynperm", 7) == 0) {
			vol->dynperm = true;
		} else if (strnicmp(data, "nodynperm", 9) == 0) {
			vol->dynperm = false;
		} else if (strnicmp(data, "nohard", 6) == 0) {
			vol->retry = 0;
		} else if (strnicmp(data, "nosoft", 6) == 0) {
			vol->retry = 1;
		} else if (strnicmp(data, "nointr", 6) == 0) {
			vol->intr = 0;
		} else if (strnicmp(data, "intr", 4) == 0) {
			vol->intr = 1;
		} else if (strnicmp(data, "nostrictsync", 12) == 0) {
			vol->nostrictsync = 1;
		} else if (strnicmp(data, "strictsync", 10) == 0) {
			vol->nostrictsync = 0;
		} else if (strnicmp(data, "serverino", 7) == 0) {
			vol->server_ino = 1;
		} else if (strnicmp(data, "noserverino", 9) == 0) {
			vol->server_ino = 0;
		} else if (strnicmp(data, "cifsacl", 7) == 0) {
			vol->cifs_acl = 1;
		} else if (strnicmp(data, "nocifsacl", 9) == 0) {
			vol->cifs_acl = 0;
		} else if (strnicmp(data, "acl", 3) == 0) {
			vol->no_psx_acl = 0;
		} else if (strnicmp(data, "noacl", 5) == 0) {
			vol->no_psx_acl = 1;
#ifdef CONFIG_CIFS_EXPERIMENTAL
		} else if (strnicmp(data, "locallease", 6) == 0) {
			vol->local_lease = 1;
#endif
		} else if (strnicmp(data, "sign", 4) == 0) {
			vol->secFlg |= CIFSSEC_MUST_SIGN;
		} else if (strnicmp(data, "seal", 4) == 0) {
			/* we do not do the following in secFlags because seal
			   is a per tree connection (mount) not a per socket
			   or per-smb connection option in the protocol */
			/* vol->secFlg |= CIFSSEC_MUST_SEAL; */
			vol->seal = 1;
		} else if (strnicmp(data, "direct", 6) == 0) {
			vol->direct_io = 1;
		} else if (strnicmp(data, "forcedirectio", 13) == 0) {
			vol->direct_io = 1;
		} else if (strnicmp(data, "noac", 4) == 0) {
			printk(KERN_WARNING "CIFS: Mount option noac not "
				"supported. Instead set "
				"/proc/fs/cifs/LookupCacheEnabled to 0\n");
		} else
			printk(KERN_WARNING "CIFS: Unknown mount option %s\n",
						data);
	}
	if (vol->UNC == NULL) {
		if (devname == NULL) {
			printk(KERN_WARNING "CIFS: Missing UNC name for mount "
						"target\n");
			return 1;
		}
		if ((temp_len = strnlen(devname, 300)) < 300) {
			vol->UNC = kmalloc(temp_len+1, GFP_KERNEL);
			if (vol->UNC == NULL)
				return 1;
			strcpy(vol->UNC, devname);
			if (strncmp(vol->UNC, "//", 2) == 0) {
				vol->UNC[0] = '\\';
				vol->UNC[1] = '\\';
			} else if (strncmp(vol->UNC, "\\\\", 2) != 0) {
				printk(KERN_WARNING "CIFS: UNC Path does not "
						    "begin with // or \\\\ \n");
				return 1;
			}
			value = strpbrk(vol->UNC+2, "/\\");
			if (value)
				*value = '\\';
		} else {
			printk(KERN_WARNING "CIFS: UNC name too long\n");
			return 1;
		}
	}
	if (vol->UNCip == NULL)
		vol->UNCip = &vol->UNC[2];

	if (uid_specified)
		vol->override_uid = override_uid;
	else if (override_uid == 1)
		printk(KERN_NOTICE "CIFS: ignoring forceuid mount option "
				   "specified with no uid= option.\n");

	if (gid_specified)
		vol->override_gid = override_gid;
	else if (override_gid == 1)
		printk(KERN_NOTICE "CIFS: ignoring forcegid mount option "
				   "specified with no gid= option.\n");

	return 0;
}
