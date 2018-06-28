Libconfig is a simple library for processing structured configuration files

Libconfig is distributed under the terms of the GNU Lesser General Public License
which is supplied in the  COPYING.LIB file.

The source code can be found at
	http://www.hyperrealm.com/libconfig/
	

The lib_vs2008 and lib_vs2010 folders contain the binary versions
(lib and dll) for libconfig compiled with VisualStudio 2008 and 2010
which are needed to compile the OAR support lib.
 

====================================================================
Generally, to comile the source of libconfig with any VisualStudio 
version, one needs to modifiy the following function of libconfig.c 
as shown below:

int config_read_file(config_t *config, const char *filename)
{
  int ret, ok = 0;

  FILE *stream = fopen(filename, "rt");
  if(stream != NULL)
  {
    // On some operating systems, fopen() succeeds on a directory.
    int fd = fileno(stream);

#if 0
    struct stat statbuf;

    if(fstat(fd, &statbuf) == 0)
    {
      // Only proceed if this is not a directory.
      if(!S_ISDIR(statbuf.st_mode))
        ok = 1;
    }
  }
#else
    struct _stat32 statbuf;

    if(_fstat32(fd, &statbuf) == 0)
    {
      // Only proceed if this is not a directory.
      if ( (statbuf.st_mode & _S_IFDIR)==0)
	ok = 1;
    }
  }
#endif

  if(!ok)
  {
    if(stream != NULL)
      fclose(stream);

    config->error_text = __io_error;
    config->error_type = CONFIG_ERR_FILE_IO;
    return(CONFIG_FALSE);
  }

  ret = __config_read(config, stream, filename, NULL);
  fclose(stream);

  return(ret);
}
