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

/* Returns the next token without changing currentToken */
Token peekToken() {
	return tokenList[counter];
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
			Empty();
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

			// consumeToken(); OptParameterList consumes past the ], so we don't have to.
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
		Token nextToken = peekToken();
		if (nextToken.val == "]") {
			/* No parameters; do nothing. */
			if (syntaxSwitch) cout << "<Opt Parameter List> ::=  <Empty>" << endl;
			Empty();
		}
		else if (nextToken.type == Identifier) {
			if (syntaxSwitch) cout << "<Opt Parameter List> ::=  <Parameter List>" << endl;

			/* We have some identifiers */
			ParameterList();
		}
		else {
			/* Something else we didn't expect... */
			cout << "Expected parameters or ] but got " << peekToken().val << endl;
			exit(-1);
		}
	}

	void ParameterList() {
		while (currentToken.val != "]") {
			if (currentToken.val == "," && peekToken().type != Identifier) {
				// This checks for this case:
				// [ numA : int, ]
				cout << "Badly formed parameter list." << endl;
				exit(-1);
			}

			Parameter();
			consumeToken();
		}

		if (syntaxSwitch) cout << "<Parameter>    |     <Parameter> , <Parameter List>" << endl;
	}

	void Parameter() {
		IDs();

		consumeToken();
		if (currentToken.val != ":") {
			cout << "Expected : but got " << peekToken().val << endl;
			exit(-1);
		}

		Qualifier();

		if (syntaxSwitch) cout << "<IDs> : <Qualifier>" << endl;
	}

	void Qualifier() {
		consumeToken();
		if (currentToken.type != Keyword) {
			cout << "Expected int | boolean | real but got " << peekToken().val << endl;
			exit(-1);
		}

		if (syntaxSwitch) cout << "<Qualifier> ::= int     |    boolean    |  real " << endl;
	}
	
	void Body() {
		consumeToken();
		if (currentToken.val != "{") {
			cout << "Expected { in function body" << endl;
			exit(-1);
		}

		// StatementList();
		while (currentToken.val != "}") {
			// TODO: If we hit the end of the file without finding }, that's an error.
			Statement();
			consumeToken();
		}

		if (currentToken.val != "}") {
			cout << "Expected } in function body" << endl;
			exit(-1);
		}

		if (syntaxSwitch) cout << "R9. <Body>  ::=  {  < Statement List>  }" << endl;
	}


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

	void IDs() {
		consumeToken();
		if (currentToken.type != Identifier) {
			cout << "Expected identifier but got " << peekToken().val << endl;
			exit(-1);
		}

		if (syntaxSwitch) cout << "<IDs> ::=     <Identifier>    | <Identifier>, <IDs>" << endl;
	}

	void StatementList() {}
	void Statement() {
		consumeToken();
		if (currentToken.type == Keyword) {
			if (currentToken.val == "return") {
				Return();
			}
			else if (currentToken.val == "if") {
				If();
			}
			else if (currentToken.val == "put") {
				Print();
			}
			else if (currentToken.val == "get") {
				Scan();
			}
			else if (currentToken.val == "while") {
				While();
			}
			else {
				cout << "Expected a token of return|if|put|get|while got " << currentToken.val << endl;
				exit(-1);
			}
		}
		else if (currentToken.type == Separator) {
			Compound();
		} else if (currentToken.type == Identifier) {
			Assign();
		} else {
			cout << "Expected a statement got " << currentToken.val << endl;
		}
	}

	void Compound() {}
	void Assign() {}

	void If() {}
	void Return() {
		if (currentToken.val != "return") {
			cout << "This shouldn't happen." << endl;
			exit(-1);
		}

		consumeToken();
		if (currentToken.type == Identifier) {
			if (syntaxSwitch) cout << "<Return> ::=  return <Expression> ;" << endl;

			Expression();

			consumeToken();
			if (currentToken.type != Separator && currentToken.val != ";") {
				cout << "Expected a ; got a " << currentToken.val << endl;
				exit(-1);
			}
		}
		else if (currentToken.type == Separator && currentToken.val == ";") {
			if (syntaxSwitch) cout << "<Return> ::=  return ;" << endl;
		}
		else {
			cout << "Expected an identifier or ; got " << currentToken.val << endl;
			exit(-1);
		}
	}
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
		if (syntaxSwitch) cout << "<Empty>   ::= "  << endl;
	}
};

#endif
