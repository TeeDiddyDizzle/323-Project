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
	space,

	Initial,
	IdentStart,
	IdentConsume,
	DollarSign,
	//Identifier,

	Number,
	NumberConsume,
	RealConsume,
	//Integer,
	//Real,

	End,
	Error
};

string tokenStrings[] = { "Operator", "Keyword", "Identifier", "Separator", "Comment", "Integer", "Real", "space",
	"Initial",
	"IdentStart",
	"IdentConsume",
	"DollarSign",
	"Number",
	"NumberConsume",
	"RealConsume",
	"End",
	"Error" };
string getTokenType(TokenType type) {
	return tokenStrings[type];
}

struct Token {
	TokenType type;

	string val;
};

#endif