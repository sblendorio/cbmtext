#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <unordered_map>
#include <set>
#include <string.h>
#include <ctype.h>
#include "utf8.h"
using namespace std;

#include "chardefs.h"

void usage(int argc, char* argv[]);
istream* openStream(char* fileName);
void closeStream(istream* file);
void printText(ostream& out, istream& in);
string filterValidCharacters(string line, int line_count);
char* singleChar(uint32_t c, char *code);
char* glyph(std::list<char> chardef, char *code);
char* translate(uint32_t c, char *code);
bool isSpace(uint32_t c);
bool isZeroWidth(uint32_t c);

int main(int argc, char* argv[]) {
    usage(argc, argv);
    istream* input = openStream(argv[1]);    
    printText(cout, *input);
	closeStream(input);
	return 0;
}

void printText(ostream& out, istream& in) {
	char code[16];
    string rawline;
    unsigned line_count = 1;

	while (getline(in, rawline)) {
		string line = filterValidCharacters(rawline, line_count);
		out << (char) 17;
		string::iterator b = line.begin();
		string::iterator e = line.end();
		while ( b != e ) {
			uint32_t cp = utf8::next(b,e);
			if (!isZeroWidth(cp)) out << translate(cp, code);
		}
		out << (char) 13;
		++line_count;
	}
	out << (char) 145;
}

char* translate(uint32_t c, char *code) {
	if (isSpace(c)) c = 32;
	if (c >= 32 && c <= 127) {
		if (c >= 'A' && c <= 'Z') return singleChar(tolower(c), code);
		if (c >= 'a' && c <= 'z') return singleChar(toupper(c), code);
		list<char> chardef = charmap[c];
		if (chardef.size() == 0) return singleChar(c, code);
		if (chardef.size() == 1) return singleChar(chardef.front(), code);
		return glyph(chardef, code);
	}
	list<char> chardef = charmap[c];
	if (chardef.size() == 0) return glyph(missing_glyph, code);
	if (chardef.size() == 1) return singleChar(chardef.front(), code);
	return glyph(chardef, code);
}

bool isSpace(uint32_t c)     {
	static set<uint32_t> spaces =
	{ 0xa0, 0x1680, 0x180e, 0x2000, 0x2001, 0x2002, 0x2003, 0x2004, 0x2005,
	        0x2006, 0x2007, 0x2008, 0x2009, 0x200a, 0x202f, 0x205f, 0x3000 };
	return spaces.find(c) != spaces.end();
}

bool isZeroWidth(uint32_t c) { return c == 0x200b || c == 0xfeff; }

char* singleChar(uint32_t c, char *code) {
	code[0]=c;
	code[1]='\0';
	return code;
}

char* glyph(std::list<char> chardef, char *code) {
  static int pow[] = {1, 2, 4, 8, 16, 32, 64, 128};
  int k=0;
  code[0]=8;
  for (int j=0; j<6; j++) {
    int p=pow[5-j];
    char value = 0;
    int i=0;
    std::list<char>::iterator it;
    for (it = chardef.begin(); it != chardef.end() && i<7; ++it) {
        int pixel = (*it & p);
        value += pow[i] * (pixel ? 1 : 0);
        ++i;
    }
    code[++k] = value + 128;
  }
  code[++k]=15;
  code[++k]=0;
  return code;
}

string filterValidCharacters(string line, int line_count) {
	string::iterator end_it = utf8::find_invalid(line.begin(), line.end());
	if (end_it != line.end()) {
		cerr << "Invalid UTF-8 sequence detected at line " << line_count << "\n";
		cerr << "This part is fine: " << string(line.begin(), end_it) << "\n";
	}
	return string(line.begin(), end_it);
}

istream* openStream(char* fileName) {
	if (!strcmp("-", fileName)) return &cin;
	ifstream* file = new ifstream(fileName);
	if (!file->is_open()) {
	    cerr << "Could not open " << fileName << endl;
		exit(2);
	}
	return file;
}

void closeStream(istream* file) {
	if (file == NULL) return;
    if (ifstream* v = dynamic_cast<ifstream*>(file)) {
    	v->close();
    	delete v;
    }
}

void usage(int argc, char* argv[]) {
	if (argc > 1) return;
	cerr << "cbmtext 0.1" << endl << endl;
	cerr << "Usage: " << (argc == 0 ? "cbmtext" : argv[0]) << " <filename>" << endl;
	cerr << "   or: " << (argc == 0 ? "cbmtext" : argv[0]) << " -" << endl << endl;
	cerr << "Outputs raw bytes for printing UTF-8 text on a CBM MPS 803 compatible printer." << endl;
	cerr << "It should be used with opencbm package by Spiro Trikaliotis to redirect the" << endl;
	cerr << "output to the printer, connected through a XUM1541 cable or Zoom Floppy." << endl;
	cerr << "If given filename is '-' (dash), then the standard input will be read." << endl;
	cerr << endl;
	cerr << "Example:" << endl << endl;
	cerr << "cbmctrl reset" << endl << "cbmctrl lock" << endl << "cbmctrl listen 4 0" << endl;
	cerr << (argc == 0 ? "cbmtext" : argv[0]) << " textfile.txt | cbmctrl write" << endl;
	cerr << "cbmctrl unlisten" << endl << "cbmctrl unlock" << endl;
	cerr << endl;
	exit(1);
}

