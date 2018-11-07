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

//Consumes next token and prints it
void consumeToken() {
	if (counter < tokenList.size()) {
		currentToken = tokenList[counter++];
		if (syntaxSwitch) {
			cout << "\n" << setw(20) << left << getTokenType(currentToken.type) << currentToken.val << endl;
		}
	}
}

//Consumes next token without printing it 
void reconsumeToken() {
	if (counter < tokenList.size()) {
		currentToken = tokenList[counter++];
	}
}
void deconsumeToken() {
	if (counter < tokenList.size()) {
		currentToken = tokenList[counter--];
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
				cout << "\t<Rat18S>  ::=   <Opt Function Definitions>   %%  <Opt Declaration List>  <Statement List> " << endl;
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
			if (syntaxSwitch) cout << "\t<Opt Function Definitions> ::= <Function Definitions>" << endl;

			FunctionDefinition(false);
			while (currentToken.val == "function") {
				FunctionDefinition(false);
			}
		}
		else {
			if (syntaxSwitch) cout << "\t<Opt Function Definitions> ::= <Empty>" << endl;
			Empty();
		}
	}

	void FunctionDefinition(bool consume = true) {
		if (consume) consumeToken();

		if (currentToken.val == "function") {
			// <Function> ::= function  <Identifier>  [ <Opt Parameter List> ]  <Opt Declaration List>  <Body>
			if (syntaxSwitch) {
				cout << "\t<Function Definitions>  ::= <Function> | <Function> <Function Definitions>" << endl;
				cout << "\t<Function> ::= function  <Identifier>  [ <Opt Parameter List> ]  <Opt Declaration List>  <Body>" << endl;
			}
			// Get the function name <Identifier>
			consumeToken();
			if (currentToken.type != Identifier) {
				cout << "Expected identifier in function definition got " << currentToken.val << endl;
				exit(-1);
			}

			consumeToken();
			if (currentToken.val != "[") {
				cout << "Expected [ in function definition got " << currentToken.val << endl;
				exit(-1);
			}

			OptParameterList();

			//consumeToken(); //OptParameterList consumes past the ], so we don't have to.
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
			if (syntaxSwitch) cout << "\t<Opt Parameter List> ::=  <Empty>" << endl;
			Empty();
		}
		else if (nextToken.type == Identifier) {
			if (syntaxSwitch) cout << "\t<Opt Parameter List> ::=  <Parameter List>" << endl;

			/* We have some identifiers */
			ParameterList();
		}
		else {
			/* Something else we didn't expect... */
			cout << "Expected parameters or ] but got " << currentToken.val << endl;
			exit(-1);
		}
	}

	void ParameterList() {
		if (syntaxSwitch) cout << "\t<Parameter List> ::= <Parameter> | <Parameter> , <Parameter List>" << endl;

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
	}

	void Parameter() {
		if (syntaxSwitch) cout << "\t<Parameter> ::= <IDs> : <Qualifier>" << endl;

		IDs();

		consumeToken();
		if (currentToken.val != ":") {
			cout << "Expected : but got " << peekToken().val << endl;
			exit(-1);
		}

		Qualifier();
	}

	void Qualifier() {
		consumeToken();
		if (syntaxSwitch) cout << "\t<Qualifier> ::= int | boolean | real" << endl;

		if (currentToken.type != Keyword) {
			cout << "Expected int | boolean | real but got " << peekToken().val << endl;
			exit(-1);
		}
	}
	
	void Body() {
		consumeToken();
		if (syntaxSwitch) cout << "\t<Body>  ::=  {  <Statement List>  }" << endl;

		if (currentToken.val != "{") {
			cout << "Expected { in function body" << endl;
			exit(-1);
		}
		StatementList();

		while (currentToken.val != "}") {
			// TODO: If we hit the end of the file without finding }, that's an error.
			Statement();
			consumeToken();
		}

		if (currentToken.val != "}") {
			cout << "Expected } in function body" << endl;
			exit(-1);
		}
		consumeToken();
	}


	void OptDeclarationList() {
		if (currentToken.val == "int" || currentToken.val == "real" || currentToken.val == "boolean") {
			if (syntaxSwitch) cout << "\t<Opt Declaration List> ::= <Declaration List> | <Empty>" << endl;
			DeclerationList();
		}
		
	}

	void DeclerationList() {
		if (syntaxSwitch) cout << "\t<Declaration List>  : = <Declaration>; | <Declaration>; <Declaration List>";
	
		Declaration();
	}
	void Declaration() {
		if (syntaxSwitch) cout << "\t<Declaration> ::=   <Qualifier > <IDs>";

	}

	void IDs() {
		if (syntaxSwitch) cout << "\t<IDs> ::=     <Identifier>    | <Identifier>, <IDs>" << endl;

		consumeToken();
		if (currentToken.type != Identifier) {
			cout << "Expected identifier but got " << peekToken().val << endl;
			exit(-1);
		}

	}

	void StatementList() {
		if (syntaxSwitch) {
			cout << "\t<Statement List> ::=   <Statement>   | <Statement> <Statement List>" << endl;
			cout << "\t<Statement> :: = <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While>" << endl;
		}
	}

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

	void If() {
		cout << "\t<If> ::= if ( <Condition> ) <Statement> endif | \n\tif (<Condition>) < Statement > else <Statement> endif" << endl;

		consumeToken();
		if (currentToken.val != "(") {
			cout << "If expected a ( but got " << currentToken.val << endl;
			exit(-1);
		}

		Condition();

		consumeToken();
		if (currentToken.val != ")") {
			cout << "If expected a ) but got " << currentToken.val << endl;
			exit(-1);
		}

		Statement();

		consumeToken();
		if (currentToken.val == "else") {
			Statement();

			consumeToken();
			if (currentToken.val == "endif") {
				if (syntaxSwitch) cout << "\t<If> ::= if  ( <Condition>  ) <Statement>   else  <Statement>  endif" << endl;
			}
			else {
				cout << "If expected else or endif but got " << currentToken.val << endl;
				exit(-1);
			}
		}
		else if (currentToken.val == "endif") {
			if (syntaxSwitch) cout << "\t<If> ::= if  ( <Condition>  ) <Statement>   endif" << endl;
		}
		else {
			cout << "If expected else or endif but got " << currentToken.val << endl;
			exit(-1);
		}
	}

	void Return() {
		if(syntaxSwitch) cout << "\t<Return> ::=  return ; |  return <Expression> ;" << endl;

		if (currentToken.val != "return") {
			cout << "This shouldn't happen." << endl;
			exit(-1);
		}

		consumeToken();
		if (currentToken.type == Identifier) {
			if (syntaxSwitch) cout << "\t<Return> ::=  return <Expression> ;" << endl;

			deconsumeToken();
			Expression();

			consumeToken();
			if (currentToken.type != Separator && currentToken.val != ";") {
				cout << "Expected a ; got a " << currentToken.val << endl;
				exit(-1);
			}
		}
		else if (currentToken.type == Separator && currentToken.val == ";") {
			if (syntaxSwitch) cout << "\t<Return> ::=  return ;" << endl;
		}
		else {
			cout << "Expected an expression or ; got " << currentToken.val << endl;
			exit(-1);
		}
	}

	void Print() {
		if (syntaxSwitch) cout << "\t<Print> ::=    put ( <Expression>);" << endl;
		
		consumeToken();
		if (currentToken.val != "put") {
			cout << "Expected keyword put, got " << currentToken.val << endl;
			exit(-1);
		}

		consumeToken();
		if (currentToken.val != "(") {
			cout << "Expected a (, got " << currentToken.val << endl;
			exit(-1);
		}

		Expression();

		consumeToken();
		if (currentToken.val != ")") {
			cout << "Expected a ), got " << currentToken.val << endl;
			exit(-1);
		}

		consumeToken();
		if (currentToken.type != Separator && currentToken.val != ";") {
			cout << "Expected a ; got a " << currentToken.val << endl;
			exit(-1);
		}
	}
	void Scan() {
		if (syntaxSwitch) cout << "\t<Scan> ::=    get ( <IDs> );";
		consumeToken();
		
		if (currentToken.val != "get") {
			cout << "Expected keyword get, got " << currentToken.val << endl;
			exit(-1);
		}
		
		if (currentToken.val == "(") {
				consumeToken();
				IDs();
				if (currentToken.val == ")") {
					consumeToken();
					if (currentToken.val == ";") {
						consumeToken();
					}
					else
					{
						cout << "Expected ';', got " << currentToken.val << endl;
						exit(-1);
					}
				}
				else {
					cout << "Expected ')', got " << currentToken.val << endl;
					exit(-1);
				}

		}
		else {
			cout << "Expected '(' " << currentToken.val << endl;
			exit(-1);
		}

		
	}
	void While() {
		if (syntaxSwitch) cout << "\t<While> ::=  while ( <Condition>  )  <Statement>" << endl;

		consumeToken();
		if (currentToken.val != "(") {
			cout << "Expected a ( got " << currentToken.val << endl;
			exit(-1);
		}

		Condition();
		Statement();
	}

	void Condition() {
		Expression();
		Relop();
		Expression();

		if (syntaxSwitch) cout << "\t<Condition> ::= <Expression> <Relop> <Expression>" << endl;
	}

	void Relop() {
		consumeToken();
		if (currentToken.val == "==" || currentToken.val == "^=" || currentToken.val == ">" ||
			currentToken.val == "<" || currentToken.val == "=>" || currentToken.val == "=<") {
			if (syntaxSwitch) cout << "\t<Relop> ::= == | ^= | > | < | => | =<" << endl;
		}
		else {
			cout << "Expected a relop" << endl;
			exit(-1);
		}
	}

	bool isRelop(string strIn) {
		return (strIn == "==" || strIn == "^=" || strIn == ">" || strIn == "<" || strIn == "=>" || strIn == "=<");
	}

	void Expression() {
		consumeToken();

		/* What tokens can we use to determine when an expression is done being parsed 
		 *  1) ')'
		 *  2) ';'
		 *  3) <Relop>
		 * Once we're in an expression, we only accept <Primary>'s
		 */


		int tokensConsumed = 0;
		while (currentToken.val != ")" && currentToken.val != ";" && !isRelop(currentToken.val)) {
			consumeToken();

			tokensConsumed++;
		}

		if (tokensConsumed == 0) {
			cout << "Expected an expression got " << currentToken.val << endl;
			exit(-1);
		}

		if (syntaxSwitch) cout << "\t<Expression> ::= <Expression> + <Term> | <Expression> - <Term> | <Term>" << endl;

		deconsumeToken();
	}

	void Term() {
		
	}

	void Factor() {
		
	}


	void Primary() {
		consumeToken();

		// <Primary> ::=     <Identifier>  |  <Integer>  |   <Identifier>  ( <IDs> )   |   ( <Expression> )   | <Real> | true | false
		if (currentToken.type == Identifier || currentToken.type == Integer || currentToken.type == Real) {
			if (syntaxSwitch) cout << "\t<Primary> ::=     <Identifier>  |  <Integer>  |   <Identifier>  ( <IDs> )   |   ( <Expression> )   | <Real> | true | false" << endl;
		}
		else if (currentToken.type == Keyword && (currentToken.val == "false" || currentToken.val == "true")) {
			if (syntaxSwitch) cout << "\t<Primary> ::=     <Identifier>  |  <Integer>  |   <Identifier>  ( <IDs> )   |   ( <Expression> )   | <Real> | true | false" << endl;
		}
		else {
			// Invalid <Primary>
			exit(-1);
		}
	}

	void Empty() {
		if (syntaxSwitch) cout << "\t<Empty>   ::= "  << endl;
	}
};

#endif
