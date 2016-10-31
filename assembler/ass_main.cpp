#include<string>
#include<map>

#include<cstdio>
#include<cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define MV2TKN(p) while(*p == ' ' || *p == '\t') p++
#define MV2SP(p) while(*p != ' ' && *p != '\t' && *p != '\n') p++
#define MV2NXTKN(p) while(*p != ' ' && *p != '\t'  && *p != '\n') p++; while(*p == ' ' || *p == '\t') p++
#define GO2EL(p) while(*p != '\n') p++

using namespace std;



int LN;
int TA, DA;
int STATE;
char *TEX, *DAT;
typedef map<string, void (*) (char *) >::iterator mi;
extern map<string, void (*) (char *) > SZM;
extern map<string, void (*) (char *) > OPM;
map<string, int> LBM;


int main(int argc, char *argv[]) {
	int fd, len, r, rv, tfs, dfs;
	struct stat statbuf;
	char *fc, *p;
	if (argc <= 1) {
		puts("too few arguments");
		return 1;
	}
	fd = open(argv[1], O_RDONLY);
	if (fd <= 0) {
		perror("main");
		return 1;
	}
	if (fstat(fd, &statbuf) != 0) {
		perror("main");
		return 1;
	}
	len = statbuf.st_size;
	fc = (char*) malloc(len+1);
	if (fc == NULL) {
		puts("memory allocation error");
		return 1;
	}
	p = fc;
	for(r = len; r > 0;) {
		rv = read(fd, p, r);
		if (rv < 0) {
			perror("main");
			return 1;
		}
		r -= rv;
		p += rv;
	}
	close(fd);
	*p = '\n'; // p == fc + len at this point
	/* delete all comments */
	for(p = fc; p <= fc + len; p++) {
		if (p[0] == '#') {
			do {*p = ' '; p++;} while (*p != '\n');
		}
	}
	
	
	try {
		LN = 0; TA = 0; DA = 0; STATE = 0;
		for(p = fc; p <= fc + len; p++) {
			char c, *q;
			mi i;
			LN++;
		
			MV2TKN(p);
			if (*p == '\n') continue;
			q = p;
			MV2SP(q);
			c = *q;
			*q = '\0';
			i = SZM.find(p);
			*q = c;
			if (i != SZM.end()) {
				MV2NXTKN(q);
				(i->second)(q);
			} else {
				q--;
				if (*q == ':' && p < q) {
					*q = '\0';
					LBM[p] = STATE == 1 ? TA : DA;
					*q = ':';
					for(q++; *q != '\n'; q++)
						if (*q != ' ' && *q != '\t')
							throw "unknown arguments";
				} else {
					throw "unknow operator";
				}
			}
			p = q;
			GO2EL(p);
		}
	}
	catch (char const *s) {
		printf("%d: %s\n", LN, s);
		return 1;
	}
	catch (...) {
		puts("some error has occurred");
		return 1;
	}

	tfs = TA*4; dfs = DA*4;
	TEX = (char*)calloc(tfs+1, 1);
	DAT = (char*)calloc(dfs+1, 1);
	if (TEX == NULL || DAT == NULL) {
		puts("memory allocation error");
		return 1;
	}
	
	try {
		LN = 0; TA = 0; DA = 0; STATE = 0;
		for(p = fc; p <= fc + len; p++) {
			char c, *q;
			mi i;
			LN++;
		
			MV2TKN(p);
			if (*p == '\n') continue;
			q = p;
			MV2SP(q);
			c = *q;
			*q = '\0';
			i = OPM.find(p);
			*q = c;
			if (i != OPM.end()) {
				MV2NXTKN(q);
				(i->second)(q);
			}
			p = q;
			GO2EL(p);
		}
	}
	catch (char const *s) {
		printf("%d: %s\n", LN, s);
		return 1;
	}
	catch (...) {
		puts("some error has occurred");
		return 1;
	}
	
	string file_name;
	size_t i1;
	file_name = argv[1];
	i1 = file_name.find_last_of(".");
	if (i1 != string::npos)
		file_name = file_name.substr(0, i1) + "_text.dat";
	else
		file_name = file_name + "_text.dat";
	fd = open(file_name.c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0777);
	if (fd < 0) {
		perror("main");
		return 1;
	}
	p = TEX;
	for(r = tfs; r > 0;) {
		rv = write(fd, p, r);
		if (rv < 0) {
			perror("main");
			return 1;
		}
		r -= rv;
		p += rv;
	}
	close(fd);
	file_name = argv[1];
	i1 = file_name.find_last_of(".");
	if (i1 != string::npos)
		file_name = file_name.substr(0, i1) + "_data.dat";
	else
		file_name = file_name + "_data.dat";
	fd = open(file_name.c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0777);
	if (fd < 0) {
		perror("main");
		return 1;
	}
	p = DAT;
	for(r = dfs; r > 0;) {
		rv = write(fd, p, r);
		if (rv < 0) {
			perror("main");
			return 1;
		}
		r -= rv;
		p += rv;
	}
	close(fd);
	free(fc); free(TEX); free(DAT);
	return 0;
}
