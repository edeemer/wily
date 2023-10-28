/*******************************************
 *	File name completion utils
 *******************************************/

#include "wily.h"
#include "text.h"
#include "data.h"
#include "view.h"
#include <libgen.h>

static void		complete(char *dir, char *file, char *res);
static void		cdirname(char *s, char *out);
static void		cbasename(char *s, char *out);
static unsigned int	nmatch(char *a, char *b);
static void		suggest(struct dirent **files, int n, char *name);

/* perform file name completion */
void
completename(View*v) {
	Path cwd;
	Path cat;
	Path path;
	Path res;
	Path dir, file;
	char *name;
	int n;
	Range r = v->sel;

	r.p0 = text_startofname(v->t, r.p0);
	name = text_duputf(v->t, r);
	if(name[0] == '/' || name[0] == '~' || name[0] == '$') {
		if(!strchr(name, '/'))
			goto ret;
		label2path(path, name);
	} else {
		if(v->t->data) {
			label2path(cwd, v->t->data->label);
			dirnametrunc(cwd);
		}
		else
			strcpy(cwd, wilydir);
		n = snprintf(path, sizeof path, "%s%s", cwd, name);
		if(n < 0)
			goto ret;
	}
	cdirname(path, dir);
	cbasename(path, file);
	complete(dir, file, res);
	if(*res) {
		n = snprintf(cat, sizeof(cat), "%s%s", dir, res);
		if(n < 0)
			goto ret;
		r.p0 = r.p1 - utflen(file);
		text_replaceutf(v->t, r, res);
	}
ret:
	free(name);
}

static void
cdirname(char *s, char *out) {
	int len;

	if(!*s) {
		strcpy(out, ".");
		return;
	}
	strcpy(out, s);
	len = strlen(s);
	len--;
	if(out[len] == '/')
		return;
	for(; len >= 0; len--) {
		if(out[len] == '/') {
			out[len+1] = '\0';
			break;
		}
	}
}

static void
cbasename(char *s, char *out) {
	int len;

	len = strlen(s) - 1;
	for(; len >= 0; len--) {
		if(s[len] == '/') {
			strcpy(out, s + len + 1);
			return;
		}
	}
	strcpy(out, s);
}

/* return completed file name; print suggestions */
static void
complete(char *dir, char *name, char *res) {
	struct dirent *last=NULL, **dirs;
	char *minlast = NULL;
	int i, nfile, lastlen;
	unsigned int ngood = 0, minmatch = ~0, match;

	if((nfile = scandir(dir, &dirs, NULL, &alphasort)) < 0) {
		*res = '\0';
		return;
	}
	for(i = 0; i < nfile; i++) {
		if(strcmp(dirs[i]->d_name, ".") == 0)
			continue;
		if(strcmp(dirs[i]->d_name, "..") == 0)
			continue;
		if(strstr(dirs[i]->d_name, name) == dirs[i]->d_name) {
			ngood++;
			last = dirs[i];
			if(minlast) {
				match = nmatch(dirs[i]->d_name, minlast);
				if(match < minmatch) {
					minmatch = match;
					minlast = dirs[i]->d_name;
				}
			} else {
				minlast = dirs[i]->d_name;
			}
		}
	}
	if(ngood == 0) {
		*res = '\0';
	} else if(ngood == 1) {
		lastlen = strlen(last->d_name);
		if(lastlen + 2 >= sizeof(Path)) {
			*res = '\0';
		}
		else {
			strncpy(res, last->d_name, lastlen);
			res[lastlen] = last->d_type&DT_DIR ? '/' : ' ';
			res[lastlen+1] = '\0';
		}
	} else {
		if(minmatch == strlen(name))
			suggest(dirs, nfile, name);
		strncpy(res, last->d_name, MIN(minmatch, sizeof(Path)));
		res[minmatch] = '\0';
	}
	for(i = 0; i < nfile; i++)
		free(dirs[i]);
	free(dirs);
}

/* print suggestions to +Errors */
static void
suggest(struct dirent **d, int n, char *name) {
	char amount[32];
	char nl[] = "\n";
	int i;

	if(strcmp(name, "") == 0 && n > 64) {
		if(snprintf(amount, sizeof(amount), "[%d files]\n", n-2) < 0)
			return;
		noutput(0, amount, strlen(amount));
		return;
	}
	for(i = 0; i < n; i++) {
		if(strcmp(d[i]->d_name, ".") == 0)
			continue;
		if(strcmp(d[i]->d_name, "..") == 0)
			continue;
		if(strstr(d[i]->d_name, name) == d[i]->d_name) {
			noutput(0, d[i]->d_name, strlen(d[i]->d_name));
			noutput(0, nl, 1);
		}
	}
}

/* return size of matching parts */
static unsigned int
nmatch(char *a, char *b) {
	Rune ra, rb;
	unsigned int ret = 0;
	int n;

	while(*a && *b) {
		n = chartorune(&ra, a);
		chartorune(&rb, b);
		if(ra != rb)
			break;
		a += n;
		b += n;
		ret += n;
	}
	return ret;
}
