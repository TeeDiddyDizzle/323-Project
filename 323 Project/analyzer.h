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


void consumeToken() {
	if (counter < tokenList.size()) {
		currentToken = tokenList[counter++];
		if (syntaxSwitch) {
			cout << "\n" << getTokenType(currentToken.type) << setw(15) << currentToken.val << endl;
		}
	}
}

class Analyzer
{
public:
	//analyzer() {}
	void Rat18S() {
		// Check for any function defs before the %% separator
		OptFunctionDefinitions();

		if (currentToken.val == "%%")
		{
			if (syntaxSwitch) {
				cout << "<Rat18S>  ::=   <Opt Function Definitions>   %%  <Opt Declaration List>  <Statement List> " << endl;
			}

			consumeToken();
			OptDeclarationList();
		}
		else
		{
			cout << "Expected %% got " << currentToken.val << endl;
			exit(-1);
		}
	}
	
	void OptFunctionDefinitions() {
		consumeToken();

		if (currentToken.val != "%%") {
			if (syntaxSwitch) cout << "<Opt Function Definitions> ::= <Function Definitions>" << endl;

			FunctionDefinition(false);
			while (currentToken.val == "function") {
				FunctionDefinition();
			}
		}
		else {
			if (syntaxSwitch) cout << "<Opt Function Definitions> ::= <Empty>" << endl;
		}
	}

	void FunctionDefinition(bool consume = true) {
		if (consume) consumeToken();

		if (currentToken.val == "function") {
			// <Function> ::= function  <Identifier>  [ <Opt Parameter List> ]  <Opt Declaration List>  <Body>
			if (syntaxSwitch) cout << "<Function> ::= function  <Identifier>  [ <Opt Parameter List> ]  <Opt Declaration List>  <Body>" << endl;

			// Get the function name <Identifier>
			Primary();

			consumeToken();
			if (currentToken.val != "[") {
				cout << "Expected [ in function definition got " << currentToken.val << endl;
				exit(-1);
			}

			OptParameterList();

			consumeToken();
			if (currentToken.val != "]") {
				cout << "Expected ] in function definition got " << currentToken.val << endl;
				exit(-1);
			}

			OptDeclarationList();

			Body();
		}
		else {
			// Not a function def
			cout << "Expected function definition got " << currentToken.val << endl;
			exit(-1);
		}
	}

	void Function() {
		// Note: Is this necessary? Only place functions are defined is before the %%
		//       so it's kinda weird to split this logic up when it's always gonna be
		//       in a function definition.
	}

	void OptParameterList() {
			
	}

	void ParameterList() {}
	void Parameter() {}
	void Qualifier() {}
	void Body() {}
	void OptDeclarationList() {
		if (syntaxSwitch) {
			cout << "<Opt Declaration List> ::= <Declaration List> | <Empty>" << endl;
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
	void Primary() {
		consumeToken();

		// <Primary> ::=     <Identifier>  |  <Integer>  |   <Identifier>  ( <IDs> )   |   ( <Expression> )   | <Real> | true | false
		if (currentToken.type == Identifier || currentToken.type == Integer || currentToken.type == Real) {
			if (syntaxSwitch) cout << "<Primary> ::=     <Identifier>  |  <Integer>  |   <Identifier>  ( <IDs> )   |   ( <Expression> )   | <Real> | true | false" << endl;
		}
		else if (currentToken.type == Keyword && (currentToken.val == "false" || currentToken.val == "true")) {
			if (syntaxSwitch) cout << "<Primary> ::=     <Identifier>  |  <Integer>  |   <Identifier>  ( <IDs> )   |   ( <Expression> )   | <Real> | true | false" << endl;
		}
		else {
			// Invalid <Primary>
			exit(-1);
		}
	}
	void Empty() {
		if (syntaxSwitch)
			cout << "<Empty>   ::= "  << endl;

	}

};







#endif
