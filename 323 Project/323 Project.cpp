// 323 Project.cpp : This file contains the 'main' function. Program execution begins and ends there
using namespace std;

#include "pch.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>

#include "language.h"
#include "token.h"

// FSM for Identifier, Integer and Real
enum State {
	Initial,

	IdentifierStart,
	IdentifierConsume,
	IdentifierEnd,

	Number,
	NumberConsume,
	RealConsume,
	Integer,
	Real,

	End,
	Error
};



Token determineToken(string buffer) {
	// If it's not a keyword or operator, run our FSM to
	// determine if it's an integer, real or identifier
	Token out;

	if (isKeyword(buffer) || isOperator(buffer)) {
		out.type = isKeyword(buffer) ? Keyword : Operator;
		out.val = buffer;
		return out;
	} else {
		



		cout << "Unhandled buffer: " << buffer << endl;
		system("pause");

		return out;
	}
}

std::vector<Token> tokenize(fstream& file) {
	std::vector<Token> tokensOut;

	string curBuffer;
	char curChar;

	while (file.good()) {
		file.get(curChar);

		if (isSeparator(string(1, curChar)) || isWhitespace(curChar)) {
			if (!isWhitespace(curChar)) {
				Token sep;
				sep.type = Separator;
				sep.val = string(1, curChar);
				tokensOut.push_back(sep);
			}

			tokensOut.push_back(determineToken(curBuffer));
			curBuffer = "";
		}
		else {
			curBuffer += curChar;
		}
	}

	return tokensOut;
}

int main()
{
	fstream infile("input.txt");

	if (infile.is_open()) {
		std::vector<Token> tokens = tokenize(infile);
		infile.close();
		// TODO: Iterate through tokens and print to console
		cout << setw(10) << "Token" << setw(10) << "Lexeme" << endl;
		for (Token curTkn : tokens) {
			cout << setw(10) << getTokenType(curTkn.type) << setw(10) << curTkn.val << endl;
		}
	}
	else {
		cout << "Error opening file." << endl;
	}

	system("pause");
}