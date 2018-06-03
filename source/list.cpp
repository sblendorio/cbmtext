#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <list>
 
char* glyph(std::list<char> chardef, char *code);

char* glyph(std::list<char> chardef, char *code) {
  static int pow[] = {1, 2, 4, 8, 16, 32, 64, 128};
  int k=0;
  code[0]=8;
  for (int j=0; j<6; j++) {
    int p=pow[5-j];
    std::stringstream ss;
    char value = 0;
    int i=0;
    std::list<char>::iterator it;
    for (it = chardef.begin(); it != chardef.end() && i<7; ++it) {
        int pixel = (*it & p);
        ss << (pixel ? "*" : ".");
        value += pow[6-i] * (pixel ? 1 : 0);
        ++i;
    }
    code[++k] = value + 128;
    std::cout << ss.str() << " " << (int) value << std::endl;
  }
  code[++k]=15;
  code[++k]='\0';
  return code;
}

int main()
{
    // Create an unordered_map of three strings (that map to strings)
    std::unordered_map<uint32_t, std::list<char>> u = {
        {1,{1,2,3}},
        {2,{4,5,6}},
        {3,{7,8,9}},
        {4,{}}
    };
    
    std::list<char> a = u[13];

    char cc[3]= {'a','b','\n'};

    std::list<char> chardef = {
      0b100011,
      0b010000,
      0b010000,
      0b001111,
      0b000000,
      0b000000,
      0b111111
    };

    char code[64];
    glyph(chardef, code);
    printf("\n\n");
    for (char *p=code; *p!='\0'; ++p) {
      printf("> %d\n",*p);
    }
}