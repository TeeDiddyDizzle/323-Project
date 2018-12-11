#pragma once

#ifndef ANALYZER_H
#define ANALYZER_H

#include <iomanip>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <list>

#include <vector>
#include "token.h"

using namespace std;

int counter = 0;
bool syntaxSwitch = true;
bool done = false;

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
	else {
		cout << "EOF" << endl;
		done = true;
	}
}

void deconsumeToken() {
	currentToken = tokenList[--counter];
}

/* Returns the next token without changing currentToken */
Token peekToken() {
	if (counter < tokenList.size()) {
		return tokenList[counter];
	}
	else {
		cout << "EOF" << endl;
		done = true;
	}
}

Token peekPrevToken() {
	if (counter > 1) {
		return tokenList[counter - 2];
	}
	else {
		cout << "This shouldn't happen" << endl;
		exit(0);
	}
}

class Analyzer
{
public:
	ofstream instructions;
	std::map<string, Symbol> symbolTable;
	int instructionCount;

	bool bufferInstructions;
	std::vector<Instruction> instructionBuffer;

	Analyzer() : instructionCount(1), bufferInstructions(false) {
		instructions.open("instructions.txt");
		if (instructions.is_open() == false) {
			cout << "Error opening instructions file for output." << endl;
			exit(errno);
		}
	}

	void enableInstructionBuffer(bool state)  {
		bufferInstructions = state;
		instructionBuffer.empty();
	}

	void printInstructionBuffer() {
		enableInstructionBuffer(false);
		for (auto instruction : instructionBuffer) {
			printInstruction(instruction.instruction, instruction.parameter);
		}
	}

	void printInstruction(std::string instruction, std::string parameter = "") {
		if (bufferInstructions) {
			instructionBuffer.push_back(Instruction{instruction, parameter});
		} else {
			instructions << instructionCount++ << " " << instruction << " " << parameter << endl;
		}
	}

	void printSymbolTable() {		
		instructions << endl << " #### SYMBOL TABLE ####" << endl;
		instructions << setw(15) << "Identifier" << setw(15) << "   Memory Location" << setw(15) << "Type" << endl;
		map<string, Symbol>::iterator it;
		for (it = symbolTable.begin(); it != symbolTable.end(); it++) {
			Symbol sym = it->second;
			instructions << setw(15) << sym.idName << setw(15) << sym.memoryLocation << setw(15) << sym.type << endl;
		}
	}

	std::string opToInstruction(std::string op) {
		std::map<string, string> mapping = {
			{"+", "ADD"},
			{"-", "SUB"},
			{"*", "MUL"},
			{"/", "DIV"},
			{">", "GRT"},
			{"<", "LES"},
			{"<", "LES"},
			{"=", "EQU"},
			{"!=", "NEQ"},
			{"=>", "GEQ"},
			{"=<", "LEQ"},
		};

		return mapping[op];
	}

	//analyzer() {}
	void Rat18S() {
		// Check for any function defs before the %% separator
		OptFunctionDefinitions();

		if (currentToken.val == "%%")
		{
			if (syntaxSwitch) {
				cout << "\t<Rat18S>  ::=   <Opt Function Definitions>   %%  <Opt Declaration List>  <Statement List> " << endl;
			}

			OptDeclarationList();
			StatementList();
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
			consumeToken();
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
			cout << "Expected : but got " << currentToken.val << endl;
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

		consumeToken();
		if (currentToken.val != "}") {
			cout << "Expected } in function body" << endl;
			exit(-1);
		}

		consumeToken();
	}


	void OptDeclarationList() {
		Token nextToken = peekToken();
		if (nextToken.val == "int" || nextToken.val == "real" || nextToken.val == "boolean") {
			if (syntaxSwitch) cout << "\t<Opt Declaration List> ::= <Declaration List> | <Empty>" << endl;
			DeclerationList();
		}

	}

	void DeclerationList() {
		if (syntaxSwitch) cout << "\t<Declaration List>  : = <Declaration>; | <Declaration>; <Declaration List>" << endl;
		consumeToken();
		Declaration();
	}

	Symbol getSymbolByName(string name) {
		if (symbolTable.count(name) == 0) {
			cout << "Identifier " << name << " has not been defined." << endl;
			exit(-1);
		}

		return symbolTable[name];
	}

	void Declaration() {
		if (syntaxSwitch) {
			cout << "\t<Declaration> ::=   <Qualifier > <IDs>" << endl;
			cout << "\t<Qualifier> ::= int | boolean | real " << endl;
		}

		if (currentToken.val != "int" && currentToken.val != "real" && currentToken.val != "boolean") {
			cout << "Expected an int | real | boolean, got " << currentToken.val;
			exit(-1);
		}

		std::string symbolType = currentToken.val;

		std::list<Token> listOfDeclarations = IDs();
		int memoryLocation = 2000;

		for (std::list<Token>::iterator it = listOfDeclarations.begin(); it != listOfDeclarations.end(); it++) {
			Token potentialIdentifier = *it;

			// Check symbol table map for similar entry
			if (symbolTable.count(potentialIdentifier.val)) {
				cout << "Identifier " << potentialIdentifier.val << " has already been defined." << endl;
				exit(-1);
			}

			// Create new symbol table entry if none exists
			Symbol newSymbol;
			newSymbol.idName = potentialIdentifier.val;
			newSymbol.memoryLocation = memoryLocation++;
			newSymbol.type = symbolType;

			symbolTable[newSymbol.idName] = newSymbol;
		}

		consumeToken();
		if (currentToken.val != ";") {
			cout << "Expected a ; but got " << currentToken.val << endl;
			exit(-1);
		}

		Token nextToken = peekToken();
		if (nextToken.val == "int" || nextToken.val == "real" || nextToken.val == "boolean") {
			consumeToken();
			Declaration();
		}
	}

	std::list<Token> IDs(int limit = -1) {
		std::list<Token> listOfIDs;
		if (currentToken.type == Identifier) listOfIDs.push_back(currentToken);

		consumeToken();
		if (syntaxSwitch) cout << "\t<IDs> ::=     <Identifier>    | <Identifier>, <IDs>" << endl;

		int curCount = 0;
		while (peekToken().val != ";" && peekToken().val != ")" && peekToken().val != ":") {
			if (currentToken.type != Identifier && currentToken.val != "," && peekToken().type != Identifier) {
				break;
			}

			if (curCount++ == limit) return listOfIDs;
			if (currentToken.type == Identifier) listOfIDs.push_back(currentToken);
			consumeToken();
		}

		if (currentToken.type == Identifier) listOfIDs.push_back(currentToken);
		if (currentToken.val == ";") deconsumeToken();
		return listOfIDs;
	}

	void StatementList() {
		if (syntaxSwitch) {
			cout << "\t<Statement List> ::=   <Statement>   | <Statement> <Statement List>" << endl;
			cout << "\t<Statement> :: = <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While>" << endl;
		}

		while (peekToken().val != "}" && !done) {
			// TODO: If we hit the end of the file without finding }, that's an error.
			Statement();
			//consumeToken();
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
		else if (currentToken.val == "{") {
			Compound();
		}
		else if (currentToken.type == Identifier && peekToken().val == "=") {
			Assign();
		}
		else if (peekPrevToken().type == Identifier && currentToken.val == "(") {
			// Function call with parameters
			if (currentToken.val == "(") {
				consumeToken();
				IDs();
				consumeToken();
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

		} else {
			cout << "Expected a statement got " << currentToken.val << endl;
			exit(-1);
		}
	}

	void Compound() {
		if (syntaxSwitch) {
			cout << "<Compound>  ::=	{ <Statement List>> } " << endl;
		}

		if (currentToken.val != "{") {
			cout << "Expected '{' got " << currentToken.val;
			exit(-1);
		}
		StatementList();

		consumeToken();
		if (currentToken.val != "}") {
			cout << "Expected } in function body" << endl;
			exit(-1);
		}

		//consumeToken();
	}

	void Assign() {
		if (syntaxSwitch) cout << "<Assign> ::= <Identifier> = <Expression> ;" << endl;

		if (currentToken.type != Identifier) {
			cout << "Expected Identifier got " << currentToken.val;
			exit(-1);
		}

		Token id = currentToken;

		consumeToken();
		if (currentToken.val != "=") {
			cout << "Expected = got " << currentToken.val;
			exit(-1);
		}

		Expression();
		printInstruction("POPM", std::to_string(getSymbolByName(id.val).memoryLocation));

		consumeToken();
		if (currentToken.val != ";") {
			cout << "Expected ; got " << currentToken.val;
			exit(-1);
		}
	}

	void If() {
		if (syntaxSwitch) cout << "\t<If> ::= if ( <Condition> ) <Statement> endif | \n\tif (<Condition>) < Statement > else <Statement> endif" << endl;

		consumeToken();
		if (currentToken.val != "(") {
			cout << "If expected a ( but got " << currentToken.val << endl;
			exit(-1);
		}

		Condition();

		enableInstructionBuffer(true);

		consumeToken();
		if (currentToken.val != ")") {
			cout << "If expected a ) but got " << currentToken.val << endl;
			exit(-1);
		}

		Statement();

		enableInstructionBuffer(false);
		instructionBuffer[0].parameter = to_string(instructionCount + instructionBuffer.size() + 1);
		printInstructionBuffer();

		consumeToken();
		if (currentToken.val == "else") {
			Statement();

			enableInstructionBuffer(false);
			instructionBuffer[0].parameter = to_string(instructionCount + instructionBuffer.size() + 1);
			printInstructionBuffer();

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
		if (syntaxSwitch) cout << "\t<Return> ::=  return ; |  return <Expression> ;" << endl;

		if (currentToken.val != "return") {
			cout << "This shouldn't happen." << endl;
			exit(-1);
		}

		consumeToken();
		if (currentToken.type == Identifier || currentToken.type == Integer || currentToken.type == Real || currentToken.val == "(") {
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

		printInstruction("STDOUT");

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


		if (syntaxSwitch) cout << "\t<Print> ::=    put ( <Expression>);" << endl;

		if (currentToken.val != "get") {
			cout << "Expected keyword get, got " << currentToken.val << endl;
			exit(-1);
		}

		consumeToken();
		if (currentToken.val != "(") {
			cout << "Expected a (, got " << currentToken.val << endl;
			exit(-1);
		}

		std::list<Token> stuffToScan = IDs();
		for (std::list<Token>::iterator it = stuffToScan.begin(); it != stuffToScan.end(); it++) {
			Token token = *it;
			printInstruction("STDIN");
			printInstruction("POPM", std::to_string(getSymbolByName(token.val).memoryLocation));
		}

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

	void While() {
		if (syntaxSwitch) cout << "\t<While> ::=  while ( <Condition> )  <Statement>" << endl;

		consumeToken();
		if (currentToken.val != "(") {
			cout << "Expected a ( got " << currentToken.val << endl;
			exit(-1);
		}

		int jumpLoc = instructionCount;
		printInstruction("LABEL");

		Condition();

		enableInstructionBuffer(true);
		printInstruction("JUMPZ", "??");	// First item in buffer

		consumeToken();
		if (currentToken.val != ")") {
			cout << "Expected a ) got " << currentToken.val << endl;
			exit(-1);
		}

		Statement();

		enableInstructionBuffer(false);
		instructionBuffer[0].parameter = to_string(instructionCount + instructionBuffer.size() + 1);
		printInstructionBuffer();

		printInstruction("JUMP", to_string(jumpLoc));
	}

	void Condition() {
		Expression();
		Token op = Relop();
		Expression();

		printInstruction(opToInstruction(op.val));

		if (syntaxSwitch) cout << "\t<Condition> ::= <Expression> <Relop> <Expression>" << endl;
	}

	Token Relop() {
		consumeToken();
		if (currentToken.val == ">" || currentToken.val == "<") {
			if (syntaxSwitch) cout << "\t<Relop> ::= == | ^= | > | < | => | =<" << endl;
			return currentToken;
		} else if (currentToken.val == "=" && peekToken().val == "=") {
			if (syntaxSwitch) cout << "\t<Relop> ::= == | ^= | > | < | => | =<" << endl;
			consumeToken();
			return Token{Operator, "=="};
		} else if (currentToken.val == "^" && peekToken().val == "=") {
			if (syntaxSwitch) cout << "\t<Relop> ::= == | ^= | > | < | => | =<" << endl;
			consumeToken();
			return Token{Operator, "^="};
		} else if (currentToken.val == "=" && peekToken().val == ">") {
			if (syntaxSwitch) cout << "\t<Relop> ::= == | ^= | > | < | => | =<" << endl;
			consumeToken();
			return Token{Operator, "=>"};
		} else if (currentToken.val == "=" && peekToken().val == "<") {
			if (syntaxSwitch) cout << "\t<Relop> ::= == | ^= | > | < | => | =<" << endl;
			consumeToken();
			return Token{Operator, "=<"};
		} else {
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

		std::list<Token> expressionTokens;

		if (syntaxSwitch) cout << "\t<Expression> ::= <Expression> + <Term> | <Expression> - <Term> | <Term>" << endl;
		
		int parenCount = -1;
		if (currentToken.val == "(") {
			parenCount = 1;
		}

		int tokensConsumed = 1;
		while (currentToken.val != ")" && currentToken.val != ";" && currentToken.val != "=" && !isRelop(currentToken.val)) {
			// Check if current token is not an operator
			// Check if next token is an operator
			// Check if next + 1 token is not an operator

			// If previous conditions are met, consume all three tokens 
			// and create an operation

			expressionTokens.push_back(currentToken);
			consumeToken();
			tokensConsumed++;

			if (currentToken.val == "(") {
				parenCount++;
			}
			else if (currentToken.val == ")") {
				parenCount--;
			}
		}

		if (expressionTokens.size() == 0) {
			cout << "Expected an expression got " << currentToken.val << endl;
			exit(-1);
		}
		else if (expressionTokens.size() == 1) {
			// Ex: "a" or "1"
			//     put(a) or put(1)

			Token toPrint = expressionTokens.front();
			if (toPrint.type != Identifier) {
				printInstruction("PUSHI", toPrint.val);
			}
			else {
				printInstruction("PUSHM", std::to_string(getSymbolByName(toPrint.val).memoryLocation));
			}
		}
		else if (expressionTokens.size() % 2 != 0) { // Cannot have an expression with even number of tokens
			// Ex: "a + b" or "a + 1 - c"

			std::list<Token>::iterator it;
			int i;
			for (it = expressionTokens.begin(), i = 0; it != expressionTokens.end(); it++, i++) {
				if (i == 0) {
					// First iteration needs to do one extra instruction
					Token lhs = *it++;
					Token relop = *it++;
					Token rhs = *it;

					if (lhs.type != Identifier) {
						printInstruction("PUSHI", lhs.val);
					}
					else {
						printInstruction("PUSHM", std::to_string(getSymbolByName(lhs.val).memoryLocation));
					}

					if (rhs.type != Identifier) {
						printInstruction("PUSHI", rhs.val);
					}
					else {
						printInstruction("PUSHM", std::to_string(getSymbolByName(rhs.val).memoryLocation));
					}

					printInstruction(opToInstruction(relop.val));
				}
				else {
					// Other iterations can use the result of the last operation 
					// that is stored at the top of the stack

					Token relop = *it++;
					Token rhs = *it;

					if (rhs.type != Identifier) {
						printInstruction("PUSHI", rhs.val);
					}
					else {
						printInstruction("PUSHM", std::to_string(getSymbolByName(rhs.val).memoryLocation));
					}

					printInstruction(opToInstruction(relop.val));
				}
			}
		}

		if (peekPrevToken().val == "(" && currentToken.val == ")") {
			// Functional call with no params
		}
		else if (parenCount == 0) {
			// Exit expression if parenCount is 0 meaning the expression was contained in parenthesis and there was the correct amount of parenthesis.
			// If it wasn't contained in parenthesis, then parenCount would stay -1
		}
		else {
			deconsumeToken();
		}
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
		if (syntaxSwitch) cout << "\t<Empty>   ::= " << endl;
	}
};

#endif
