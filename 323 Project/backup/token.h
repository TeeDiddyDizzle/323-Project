#ifndef TOKEN_H
#define TOKEN_H
using namespace std;

enum TokenType {
	Operator,
	Keyword,
	Identifier,
	Separator,
	Comment
};

string tokenStrings[] = {"Operator", "Keyword", "Identifier", "Separator", "Comment"};
string getTokenType(TokenType type) {
	return tokenStrings[type];
}

struct Token {
	TokenType type;

	string val;
};

#endif