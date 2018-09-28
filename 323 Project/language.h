#ifndef LANGUAGE_H
#define LANGUAGE_H
#include <vector>
#include "token.h"

using namespace std;

bool isOperator(string);
bool isSeparator(string);
TokenType otherToken(string);

std::vector<string> keywords = { "int", "boolean", "real", "function", "if", "endif", "else", "return", "put", "get", "while" };
std::vector<string> separators = { "(", ")", "{", "}", "[", "]", ":", ";", "%%", "," };
std::vector<string> operators = { "=<", "=>", "<", ">", "^=", "==", "=", "*", "/", "+", "-" };

bool isKeyword(string input) {
	return std::count(keywords.begin(), keywords.end(), input) > 0;
}

bool isWhitespace(char curChar) {
	return curChar == ' ' || curChar == '\t' || curChar == '\r' || curChar == '\n';
}

bool isOperator(string token) {
	bool isOp = false;
	for (int i = 0; i < operators.size(); i++) {
		if (token == operators[i]) {
			isOp = true;
		}
	}
	return isOp;
}

bool isSeparator(string token) {
	bool isSep = false;
	for (int i = 0; i < separators.size(); i++) {
		if (token == separators[i]) {
			isSep = true;
		}
	}
	return isSep;
}
#endif

TokenType type[10][5] = {
	//	A-Za-z			0-9				.				$				Whitespace
		IdentStart,		Number,			Error,			Error,			Error,				//Initial
		IdentConsume,	Error,			Error,			Error,			Error,				//IdentStart
		IdentConsume,	IdentConsume,	Error,			IdentConsume,	Identifier,			//IdentConsume
		Error,			Error,			Error,			Error,			Error,				//DollarSign
		Identifier,		Identifier,		Identifier,		Identifier,		Identifier,			//Identifier
		Error,			NumberConsume,	Error,			Error,			Integer,			//Number
		Error,			NumberConsume,	RealConsume,	Error,			Integer,			//NumberConsume
		Error,			NumberConsume,	Error,			Error,			Real,				//RealConsume
		Integer,		Integer,		Integer,		Integer,		Integer,			//Integer
		Real,			Real,			Real,			Real,			Real,				//Real
};

TokenType otherToken(string token) {
	bool isIdent = false, isInt = false, isReal = false;
	//TokenType type[11][5] = { space };
	//TokenType type = space;
	int i, j;
	if (isalpha(token[0])) {		//IdentStart 
		i = 2;						//IdentConsume
		j = 0;
		for (int a = 1; a < token.length(); a++) {
			
			if (i == 3 || i == 2) {
				if (a == token.length() - 1) {
					i = 4;			//Identifier
					return type[i][j];	//
				}
				else if (i == 3 && a != token.length() - 1) {
					return Error;
				}
			}
			
			if (isalnum(token[a])) {
				i = 2;				//IdentConsume
				if (isalpha(token[a])) {
					j = 0;			//A-Za-z
				}
				else {
					j = 1;			//0-9
				}
			}
			else if (token[a] == '$') {
				i = 3;				//DollarSign
				j = 3;				//$
			}
		}
	}
	else if (isdigit(token[0])) {	//number
		i = 6;						//NumberConsume
		j = 1;
		for (int a = 1; a < token.length(); a++) {
			
			if (i == 6 && a == token.length()-1 && isdigit(token[a])) {
				i = 8;
				j = 1;
				return type[i][j];
			}
			else if (i == 7 && a == token.length()-1 && isdigit(token[a])) {
				i = 9;
				j = 1;
				return type[i][j];
			}
			else if (a == token.length()-1 && !isdigit(token[a])) {
				return Error;
			}

			if (isdigit(token[a])) {
				j = 1;				//0-9 
									//i stays 6 if there was no decimal yet, or stays 7 if there was a decimal
			}
			else if (token[a] == '.') {
				j = 2;				//.
				i = 7;				//RealConsume
			}
		}
	}
	else
		return Error;
	
	return End;
}