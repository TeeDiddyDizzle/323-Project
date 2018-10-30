#pragma once

#ifndef ANALYZER_H
#define ANALYZER_H

#include <iostream>
using namespace std;

#include <vector>
#include "token.h"


int counter = 0;
bool syntaxSwitch = true;

vector<Token> tokenList;
Token currentToken;


void nextToken() {

	if (counter < tokenList.size()) {
		currentToken = tokenList[counter];
		if (syntaxSwitch) {
			cout << "\n" << setw(15) << getTokenType(currentToken.type) << setw(15) << currentToken.val << endl;
			counter++;
		}

	}
}

class analyzer
{
public:
	//Parser() {}
	//~Parser() {}
	void Rat18S() {
		nextToken();
		if (syntaxSwitch) {
			cout << "<Rat18S>  ::=   <Opt Function Definitions>   %%  <Opt Declaration List>  <Statement List> " << endl;
		}

		OptFunctionDefinitions();

		if (currentToken.val == "%%")
		{
			nextToken();
			OptDeclerationList();
		}
		else
		{
			exit(-1);
		}

	}
	
	void OptFunctionDefinitions() {
		if (syntaxSwitch) {
			cout << "<Opt Function Definitions> ::= <Function Definitions>     |  <Empty>" << endl;
		}

		if (currentToken.val == "function") {
			FunctionDefinition();
		}

		else if (currentToken.val == "%%") {
			Empty();
		}

		else {
			exit(-1);
		}

	
	}
	void FunctionDefinition() {
			//<Function Definitions>  ::= <Function> | <Function> <Function Definitions>"
	}
	void Function() {
			//"<Function> ::= function  <Identifier>  [ <Opt Parameter List> ]  <Opt Declaration List>  <Body>"
	}
	void OptParameterList() {}
	void ParameterList() {}
	void Parameter() {}
	void Qualifier() {}
	void Body() {}
	void OptDeclerationList() {
		if (syntaxSwitch) {
			cout << "<Opt Declaration List> ::= <Declaration List> | <Empty>" << endl;
		}
		if (currentToken.val == "{") {
			Empty();
		} 
		else {
			exit(-1);
		}
	
	}
	void DeclerationList() {}
	void Declaration() {}
	void IDs() {}
	void StatementList() {}
	void Statement() {}
	void Compound() {}
	void Assign() {}
	void If() {}
	void Return() {}
	void Print() {}
	void Scan() {}
	void While() {}
	void Condition() {}
	void Relop() {}
	void Expression() {}
	void Term() {}
	void Factor() {}
	void Primary() {}
	void Empty() {
		if (syntaxSwitch)
			cout << "<Empty>   ::= "  << endl;

	}

};







#endif