#ifndef TOKEN_H
#define TOKEN_H
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
	int memoryLocation;
};

#endif