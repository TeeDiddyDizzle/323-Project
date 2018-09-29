#ifndef LANGUAGE_H
#define LANGUAGE_H
#include <vector>
#include "token.h"

using namespace std;

bool isOperator(string);
bool isSeparator(string);
Token tokenFromFSM(string);

std::vector<string> keywords = { "int", "boolean", "real", "function", "if", "endif", "else", "return", "put", "get", "while" };
std::vector<string> separators = { "(", ")", "{", "}", "[", "]", ":", ";", "%%", ",", "!"};
std::vector<string> operators = { "=<", "=>", "<", ">", "^=", "==", "=", "*", "/", "+", "-" };

bool isWhitespace(char curChar) {
	return curChar == ' ' || curChar == '\t' || curChar == '\r' || curChar == '\n';
}

bool isKeyword(string input) {
	return std::count(keywords.begin(), keywords.end(), input) > 0;
}

bool isOperator(string token) {
	return std::count(operators.begin(), operators.end(), token) > 0;
}

bool isSeparator(string token) {
	return std::count(separators.begin(), separators.end(), token) > 0;
}

enum State {
	Initial,
	
	IdentifierStart,
	IdentifierConsume,
	IdentifierEnd,

	Number,
	NumberConsume,
	RealConsume,
	IntegerEnd,
	RealEnd,

	EndState,
	ErrorState
};

State fsmStates[9][5] = {
		//  0					1					2				3					4
		//	A-Za-z				0-9					.				$					:/Whitespace/End	State
		{ IdentifierStart,		Number,				ErrorState,		ErrorState,			ErrorState },		// Initial

		{ IdentifierConsume,	ErrorState,			ErrorState,		ErrorState,			ErrorState },		// IdentifierStart
		{ IdentifierConsume,	IdentifierConsume,	ErrorState,		IdentifierEnd,		IdentifierEnd },	// IdentifierConsume
		{ EndState,				EndState,			EndState,		EndState,			EndState },			// IdentifierEnd

		{ ErrorState,			NumberConsume,		ErrorState,		ErrorState,			IntegerEnd },		// Number
		{ ErrorState,			NumberConsume,		RealConsume,	ErrorState,			IntegerEnd },		// NumberConsume
		{ ErrorState,			RealConsume,		ErrorState,		ErrorState,			RealEnd },			// RealConsume
		{ EndState,				EndState,			EndState,		EndState,			EndState },			// Integer
		{ EndState,				EndState,			EndState,		EndState,			EndState }			// Real
};

State getNextStateFromInput(State curState, char input) {
	if (isalpha(input)) return fsmStates[curState][0];
	if (isdigit(input)) return fsmStates[curState][1];
	if (input == '.') return fsmStates[curState][2];
	if (input == '$') return fsmStates[curState][3];
	if (isWhitespace(input) || input == '\x0' || input == ':') return fsmStates[curState][4];
	return ErrorState;
}

Token tokenFromFSM(string token) {
	State curState = Initial;
	Token tokenOut;

	int curCharIdx = 0;
	while ((curState != EndState && curState != ErrorState)) {
		char curChar;
		if (curCharIdx < token.length()) {
			curChar = token[curCharIdx];
		}
		else {
			curChar = '\x0';
		}

		curState = getNextStateFromInput(curState, curChar);

		switch (curState) {
		case IdentifierEnd:
			tokenOut.val += curChar;
			tokenOut.type = Identifier;
			curState = EndState;
			break;
		case IntegerEnd:
			tokenOut.type = Integer;
			tokenOut.val += curChar;
			tokenOut.intVal = atoi(tokenOut.val.data());
			curState = EndState;
			break;
		case RealEnd:
			tokenOut.type = Real;
			tokenOut.val += curChar;
			tokenOut.floatVal = atof(tokenOut.val.data());
			curState = EndState;
			break;
		default:
			tokenOut.val += curChar;
			curCharIdx++;
			break;
		}
	}

	if (curState == ErrorState) {
		cout << "Error parsing token: " << tokenOut.val << endl;
		tokenOut.type = Error;
	}

	return tokenOut;
}

#endif