// 323 Project.cpp : This file contains the 'main' function. Program execution begins and ends there
using namespace std;

#include "pch.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>

#include "language.h"
#include "token.h"
#include "analyzer.h"

Token determineToken(string buffer) {
	// If it's not a keyword or operator, run our FSM to
	// determine if it's an integer, real or identifier
	Token out;
	if (isKeyword(buffer) || isOperator(buffer)) {
		out.type = isKeyword(buffer) ? Keyword : Operator;
		out.val = buffer;
		return out;
	} else if (isSeparator(buffer)) {
		out.type = Separator;
		out.val = buffer;
		return out;
	} else {
		return tokenFromFSM(buffer);
	}
}

std::vector<Token> tokenize(fstream& file) {
	std::vector<Token> tokensOut;

	string curBuffer;
	char curChar;

	while (file.get(curChar)) {
		// Throw out any comments
		if (curChar == '!') {
			file.get(curChar);
			while (curChar != '!') {
				file.get(curChar);
				if (curChar == '\r' || curChar == '\n') {
					cout << "Unterminated comment." << endl;
					exit(-1);
				}
			}
			continue;
		}

		if (isOperator(string(1, curChar))) {
			if (curBuffer.size() > 0) {
				Token toPush = determineToken(curBuffer);
				if (toPush.type != Error) {
					tokensOut.push_back(toPush);
					curBuffer = "";
				}
				else {
					cout << "Bad token: " << toPush.val << endl;
				}
			}

			Token sep;
			sep.type = Operator;
			sep.val = curChar;
			tokensOut.push_back(sep);
			continue;
		}

		if (isSeparator(string(1, curChar))) {
			if (curBuffer.size() > 0) {
				Token toPush = determineToken(curBuffer);
				if (toPush.type != Error) {
					tokensOut.push_back(toPush);
					curBuffer = "";
				}
				else {
					cout << "Bad token: " << toPush.val << endl;
				}
			}

			Token sep;
			sep.type = Separator;
			sep.val = curChar;
			tokensOut.push_back(sep);
			continue;
		} 
		
		if (isWhitespace(curChar)) {
			if (curBuffer != "") {
				Token toPush = determineToken(curBuffer);
				if (toPush.type != Error) {
					tokensOut.push_back(toPush);
					curBuffer = "";
				}
				else {
					cout << "Bad token: " << toPush.val << endl;
				}
			}
			continue;
		}
		
		curBuffer += curChar;
	}

	if (curBuffer.size() > 0) {
		Token toPush = determineToken(curBuffer);
		if (toPush.type != Error) {
			tokensOut.push_back(toPush);
			curBuffer = "";
		}
		else {
			cout << "Bad token: " << toPush.val << endl;
		}
	}

	return tokensOut;
}

int main(int argc, char* argv[])
{
	analyzer analyze;
	fstream infile;
	if (argc > 1) {
		infile.open(argv[1]);
	}
	else {
		infile.open("test.txt");
	}

	if (infile.is_open()) {
		std::vector<Token> tokens = tokenize(infile);
		infile.close();
		cout << setw(15) << "Token" << setw(15) << "Lexeme" << endl;
		for (Token curTkn : tokens) {
			cout << setw(15) << getTokenType(curTkn.type) << setw(15) << curTkn.val << endl;
		}
		tokenList.insert(tokenList.end(), tokens.begin(), tokens.end());
	}
	else {
		cout << "Error opening file." << endl;
	}
	analyze.Rat18S();
	system("pause");
}
