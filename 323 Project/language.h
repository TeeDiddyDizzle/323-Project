#ifndef LANGUAGE_H
#define LANGUAGE_H
#include <vector>

using namespace std;

bool isOperator(string);
bool isSeparator(string);

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