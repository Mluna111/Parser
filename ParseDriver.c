//starter code for Simple_PL1 scanner
#include <stdio.h> //for c I/o
#include <stdlib.h> // for exit()
#include <ctype.h> // for isalpha(), isalnum(), ...\

#include <string.h>

enum tokenType {
	READ, WRITE, ID, NUMBER, LPAREN, RPAREN, SEMICOLON, COMMA, ASSIGN, PLUS, MINUS, TIMES, DIV, SCAN_EOF
};

char* mnemonic[] = { "READ", "WRITE", "ID", "NUMBER", "LPAREN", "RPAREN", "SEMICOLON", "COMMA",
"ASSIGN", "PLUS", "MINUS", "TIMES", "DIV", "SCAN_EOF" };

void lexical_error(char ch)
{
	fprintf(stderr, "Lexical Error. Unexpected character: %c.\n", ch);
}

char lexeme[256] = { '\0' };
unsigned int lexLen = 0;
FILE* src;
enum tokenType scan()
{
	static int currentCh = ' ';
	static int tempCh = ' ';
	char* reserved[2] = { "read", "write" };
	lexLen = 0; // clear lexeme buffer for each scan
	lexeme[0] = '\0';
	extern FILE* src; //pointer to FILE handle that binds to source file.
	if (feof(src)) {
		return SCAN_EOF;
	}
	while ((currentCh = fgetc(src)) != EOF) {
		if (isspace(currentCh)) {
			continue;
		}
		else if (isalpha(currentCh) || currentCh == '_') { //needs to be modified
			lexeme[0] = currentCh;
			lexLen = 1;
			for (tempCh = fgetc(src); (isalnum(tempCh) || tempCh == '_');) {
				//build identifier lexeme
				lexeme[lexLen] = tempCh;
				lexLen++;
				tempCh = fgetc(src);
			}
			lexeme[lexLen] = '\0';
			ungetc(tempCh, src); //put back character that is not a alpha/digit or ‘`_’
			// see if lexeme is a reserved word, if not, return ID.
			if (strcmp(lexeme, reserved[0]) == 0) {
				return READ;

			}
			else if (strcmp(lexeme, reserved[1]) == 0) {

				return WRITE;
			}
			else {
				return ID;
			}
		}
		else if (isdigit(currentCh)) {
			// build lexeme for number
			// finish fixing lexeme string, ungetc the last character read that is not a digit and then return NUMBER
			lexeme[0] = currentCh;
			lexLen = 1;
			for (tempCh = fgetc(src); isdigit(tempCh);) {
				//build identifier lexeme
				lexeme[lexLen] = tempCh;
				lexLen++;
				tempCh = fgetc(src);
			}
			lexeme[lexLen] = '\0';
			ungetc(tempCh, src); //put back character that is not a alpha/digit or ‘`_’
			// see if lexeme is a reserved word, if not, return ID.
			return NUMBER;
		}
		else if (currentCh == '(') {  
			return LPAREN;
		}
		else if (currentCh == ')') {
			return RPAREN;
		}
		else if (currentCh == ';') {
			return SEMICOLON;
		}
		else if (currentCh == ',') {
			return COMMA;
		}
		else if (currentCh == ':') {
			tempCh = fgetc(src);
			if (tempCh == '=') {
				return ASSIGN;
			}
			else {
				//put back character that is not a alpha/digit or ‘`_’
				lexical_error(currentCh);
				ungetc(tempCh, src);
			}
		}
		else if (currentCh == '+') {
			return PLUS;
		}
		else if (currentCh == '-') {
			return MINUS;
		}
		else if (currentCh == '*') {
			return TIMES;
		}
		else if (currentCh == '/') {
			return DIV;
		}
		else {
			lexical_error(currentCh);
		}
	}
	return SCAN_EOF;
}

enum tokenType currentToken;

void parse_error(char* errMsg, char* lexeme) {
	//extern unsigned numberOfErrors; //for future if error recovery used
	//numberOfErrors++;
	fprintf(stderr, "%s: %s\n", errMsg, lexeme);
}
void match(enum tokenType expected)
{
	if (currentToken == expected) {
		//currentToken = scan();
	}
	else {
		parse_error("Expected symbol", mnemonic[expected]);
		exit(1);
	}
}



int program();
void stmt();
void expr();
void term();
void factor();



int program()
{
	currentToken = scan();
	if (currentToken == SCAN_EOF) {
		printf("Error file is empty.");
		exit(2);
	}

	do {
		stmt();
		currentToken = scan();
	} while (currentToken != SCAN_EOF);
	
	return 0;
}

void stmt()
{
	if (currentToken == ID) {
		currentToken = scan();
		match(ASSIGN);
		expr();
		match(SEMICOLON);
	}
	else if (currentToken == READ) {
		currentToken = scan();
		match(LPAREN);
		
		do{
			currentToken = scan();
			match(ID);
			currentToken = scan();					
		} while (currentToken == COMMA);
		match(RPAREN);
		currentToken = scan();
		match(SEMICOLON);
	}
	else if (currentToken == WRITE) {
		currentToken = scan();
		match(LPAREN);
		do{
			expr();
		} while (currentToken == COMMA);
		match(RPAREN);
		currentToken = scan();
		match(SEMICOLON);
	}
}

void expr()
{
	do {
		term();
	}while (currentToken == PLUS || currentToken == MINUS);
}

void term() {

	do {

		factor();
		currentToken = scan();
	} while (currentToken == TIMES || currentToken == DIV);
}

void factor() {

	currentToken = scan();

	if (currentToken == ID || currentToken == NUMBER){
		return;
	}
	else if (currentToken == LPAREN) {
		expr();
		match(RPAREN);
	}
	else {
		printf("Error in expression: Expected ID, Number, or \'(\'.");
		exit(1);
	}

}

int main(int argc, char* argv[])
{
	extern FILE* src;
	//enum tokenType currentToken;
	if (argc > 1) {//should be better testing for proper number of arguments, but not required for this project
		if (fopen_s(&src, argv[1], "r")) {
			fprintf(stderr, "Error opening source file: %s ", argv[1]);
			exit(1);
		}
	}

	program();
	printf("Parsing complete. No errors.");

	fclose(src);
	return 0;
} //end driver
