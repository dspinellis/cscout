// Add a file to the directory tree for later browsing
void *dir_add_file(Fileid f);
// Display a directory's contents
void dir_page(FILE *of, void *p);
// Display on of a URL for browsing the project's top dir
void dir_top(FILE *of, const char *name);

