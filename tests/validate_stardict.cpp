#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <string>
#include <glib.h>
#include <glib/gstdio.h>

static inline gint stardict_strcmp(const gchar *s1, const gchar *s2)
{
        gint a;
        a = g_ascii_strcasecmp(s1, s2);
        if (a == 0)
                return strcmp(s1, s2);
        else
                return a;
}

static int verify_idxfile(const gchar *idxfilename, guint ifo_wordcount, guint ifo_index_file_size)
{
	struct stat stats;
	if (g_stat(idxfilename, &stats)==-1) {
		g_print("File not exist: %s\n", idxfilename);
                return EXIT_FAILURE;
	}
	g_print("Verifing file: %s\n", idxfilename);

	if (ifo_index_file_size!=(guint)stats.st_size) {
		g_print("Error: in .ifo file, idxfilesize=%d, while the real idx file size is %ld\n", ifo_index_file_size, stats.st_size);
	}

	gchar *buffer = (gchar *)g_malloc (stats.st_size);
	gchar *buffer_end = buffer+stats.st_size;
	FILE *idxfile;
        idxfile = fopen(idxfilename,"rb");
        fread (buffer, 1, stats.st_size, idxfile);
        fclose (idxfile);

	gchar *p=buffer;
	gchar *preword=NULL;
	int wordlen;
	gint cmpvalue;
	guint32 size;
	guint wordcount=0;
	while (1) {
                if (p == buffer_end) {
                        break;
                }
		wordlen=strlen(p);
		if (wordlen==0)	{
			g_print("Error: wordlen==0\n");
		}else {
			if (wordlen>=256) {
				g_print("Error: wordlen>=256, %s\n", p);
			}
			if (g_ascii_isspace(*p)) {
				g_print("Warning: begin with space, %s\n", p);
			}
			if (g_ascii_isspace(*(p+wordlen-1))) {
				g_print("Warning: end with space, %s\n", p);
			}
		}
		if (strpbrk(p, "\t\n")) {
			g_print("Warning: contain invalid character, %s\n", p);
		}
		if (!g_utf8_validate(p, wordlen, NULL)) {
			g_print("Error: invalid utf8 string, %s\n", p);
		}
		if (preword) {
			cmpvalue=stardict_strcmp(preword, p);
			if (cmpvalue==0) {
				g_print("Error: duplicate word, %s\n", p);
			}
			else if (cmpvalue>0) {
				g_print("Error: wrong string order, %s\n", p);
			}
		}
		preword=p;
		p+=wordlen +1 + sizeof(guint32);
		size=*reinterpret_cast<guint32 *>(p);
		size=g_ntohl(size);
		if (size==0) {
			g_print("Error: definition size==0, %s\n", preword);
		}
		p+=sizeof(guint32);
		wordcount++;
	}
	if (wordcount!=ifo_wordcount) {
		g_print("Error: in .ifo file, wordcount=%d, while the real word count is %d\n", ifo_wordcount, wordcount);
	}

	g_free(buffer);
	return EXIT_SUCCESS;
}

static int verify_file(const gchar *ifofilename)
{
	if (!g_file_test(ifofilename, G_FILE_TEST_EXISTS))
		return EXIT_FAILURE;

	gchar *buffer;
	g_file_get_contents(ifofilename, &buffer, NULL, NULL);
	if (!g_str_has_prefix(buffer, "StarDict's dict ifo file\nversion=2.4.2\n")) {
		g_free(buffer);
		return EXIT_FAILURE;
	}

	gchar *p1,*p2,*p3;

	guint wordcount;
	p1 = buffer;
	p2 = strstr(p1,"\nwordcount=");
	p3 = strchr(p2+ sizeof("\nwordcount=")-1,'\n');
	gchar *tmpstr = (gchar *)g_memdup(p2+sizeof("\nwordcount=")-1, p3-(p2+sizeof("\nwordcount=")-1)+1);
	tmpstr[p3-(p2+sizeof("\nwordcount=")-1)] = '\0';
	wordcount = atol(tmpstr);
	g_free(tmpstr);

	guint index_file_size;
	p2 = strstr(p1,"\nidxfilesize=");
	p3 = strchr(p2+ sizeof("\nidxfilesize=")-1,'\n');
	tmpstr = (gchar *)g_memdup(p2+sizeof("\nidxfilesize=")-1, p3-(p2+sizeof("\nidxfilesize=")-1)+1);
	tmpstr[p3-(p2+sizeof("\nidxfilesize=")-1)] = '\0';
	index_file_size = atol(tmpstr);
	g_free(tmpstr);

	g_free(buffer);

	std::string idxfilename=ifofilename;
	idxfilename.replace(idxfilename.length()-sizeof("ifo")+1, sizeof("ifo")-1, "idx");
	return verify_idxfile(idxfilename.c_str(), wordcount, index_file_size);
}

int main(int argc,char * argv [])
{
	if (argc!=2) {
		std::cerr<<"Usage: "<<argv[0]<<" file.ifo"<<std::endl;
		return EXIT_FAILURE;
	}
		       
	return verify_file(argv[1]);
}
