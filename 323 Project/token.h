#ifndef TOKEN_H
#define TOKEN_H
#include <string>

using namespace std;

enum TokenType {
	Operator,
	Keyword,
	Identifier,
	Separator,
	Comment,
	Integer,
	Real,

	Error
};

string tokenStrings[] = { "Operator", "Keyword", "Identifier", "Separator", "Comment", "Integer", "Real", "Error" };

string getTokenType(TokenType type) {
	return tokenStrings[type];
}

struct Token {
	TokenType type;

	string val;
	double floatVal;
	int intVal;
};

struct Symbol {
	string idName;
	string type;
	int memoryLocation;
};

struct Instruction {
	string instruction;
	string parameter;
	string lineNumber;
};

#endif
