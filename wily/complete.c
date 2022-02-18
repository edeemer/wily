/*******************************************
 *	File name completion utils
 *******************************************/

#include "wily.h"
#include "text.h"
#include "data.h"
#include "view.h"
#include <libgen.h>

static char *		complete(char *dir, char *file);
static char *		cdirname(char *s);
static char *		cbasename(char *s);
static unsigned int	nmatch(char *a, char *b);
static void		suggest(struct dirent **files, int n, char *name);

/* perform file name completion */
void
completename(View*v) {
	Path cwd;
	Path cat;
	Path path;
	char *dir=0, *file=0, *res=0;
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
		label2path(cwd, v->t->data->label);
		dirnametrunc(cwd);
		n = snprintf(path, sizeof path, "%s%s", cwd, name);
		if(n < 0)
			goto ret;
	} 
	dir = cdirname(path);
	file = cbasename(path);
	res = complete(dir, file);
	if(res) {
		n = snprintf(cat, sizeof(cat), "%s%s", dir, res);
		if(n < 0)
			goto ret;
		r.p0 = r.p1 - utflen(file);
		text_replaceutf(v->t, r, res);
	}
ret:
	free(name);
	if(res)
		free(res);
	if(dir)
		free(dir);
	if(file)
		free(file);
}

static char *
cdirname(char *s) {
	char *ret;
	int len;

	if(!*s)
		return strdup(".");
	ret = strdup(s);
	len = strlen(s);
	len--;
	if(ret[len] == '/')
		return ret;
	for(; len >= 0; len--) {
		if(ret[len] == '/') {
			ret[len+1] = '\0';
			break;
		}
	}
	return ret;
}

static char *
cbasename(char *s) {
	int len;

	len = strlen(s) - 1;
	for(; len >= 0; len--) {
		if(s[len] == '/')
			return strdup(s + len + 1);
	}
	return strdup(s);
}

/* return completed file name; print suggestions */
static char *
complete(char *dir, char *name) {
	struct dirent *last=NULL, **dirs;
	char *ret;
	char *minlast = NULL;
	int i, nfile, lastlen;
	unsigned int ngood = 0, minmatch = ~0, match;

	if((nfile = scandir(dir, &dirs, NULL, &alphasort)) < 0)
		return NULL;
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
		ret = NULL;
	} else if(ngood == 1) {
		lastlen = strlen(last->d_name);
		ret = malloc(lastlen + 2);
		strncpy(ret, last->d_name, lastlen);
		ret[lastlen] = last->d_type&DT_DIR ? '/' : ' ';
		ret[lastlen+1] = '\0';
	} else {
		if(minmatch == strlen(name))
			suggest(dirs, nfile, name);
		ret = malloc(minmatch + 1);
		strncpy(ret, last->d_name, minmatch);
		ret[minmatch] = 0;
	}
	for(i = 0; i < nfile; i++)
		free(dirs[i]);
	free(dirs);
	return ret;
}

/* print suggestions to +Errors */
static void
suggest(struct dirent **d, int n, char *name) {
	char *nl, *amount;
	int i;

	nl = strdup("\n");
	amount = malloc(32);
	if(strcmp(name, "") == 0 && n > 64) {
		if(snprintf(amount, 31, "[%d files]\n", n-2) < 0)
			goto ret;
		amount[31] = '\0';
		noutput(0, amount, strlen(amount));
		goto ret;
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
ret:
	free(nl);
	free(amount);
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
