#include "stdafx.h"

#include "curve.h"

/*
	To do:
		- User definable variables
*/

double uplus(double val){ return val; }
double uminus(double val){ return -val; }

double sind(double deg){ return sin(D2R(deg)); }
double cosd(double deg){ return cos(D2R(deg)); }
double tand(double deg){ return tan(D2R(deg)); }

double asind(double x){ return R2D(asin(x)); }
double acosd(double x){ return R2D(acos(x)); }
double atand(double x){ return R2D(atan(x)); }

double sing(double deg){ return sin(G2R(deg)); }
double cosg(double deg){ return cos(G2R(deg)); }
double tang(double deg){ return tan(G2R(deg)); }

double asing(double x){ return R2G(asin(x)); }
double acosg(double x){ return R2G(acos(x)); }
double atang(double x){ return R2G(atan(x)); }


UnOp UnOps[] = {
	{ "u+", uplus, 3 },
	{ "u-", uminus, 3 },
	{ "sin", sin, 5 },
	{ "cos", cos, 5 },
	{ "tan", tan, 5 },
	{ "asin", asin, 5 },
	{ "acos", acos, 5 },
	{ "atan", atan, 5 },
	{ "ln" /* base e */, log, 5 },	//	in math.h, log(x) is natural log
	{ "log" /* base 10 */, log10, 5 },
	{ "exp" /* e^x */, exp, 5 },
	{ "sinh", sinh, 5 },
	{ "cosh", cosh, 5 },
	{ "tanh", tanh, 5 },
	{ "sqrt", sqrt, 5 },
	{ "ceil" /* round off to first greater int */, ceil, 5 },
	{ "fabs", fabs, 5 },
	{ "floor" /* round off to first lesser int (truncate) */, floor, 5 }
};

double add(double a, double b){ return a + b; }
double sub(double a, double b){ return a - b; }
double div(double a, double b){ return a / b; }
double mul(double a, double b){ return a * b; }

BinOp BinOps [] = {
	{ "+", add, 1 },
	{ "-", sub, 1 },
	{ "*", mul, 2 },
	{ "/", div, 2 },
	{ "%", fmod, 2},
	{ "^", pow, 4 }
};

void Token::dump(){
	switch(type){
	case CONSTANT:
		printf("%f\n", val);
		break;
	case VARIABLE:
		printf("%p\n", xptr);
		break;
	case UNARY:
		printf("%s %d\n", u->opr, precedence);
		break;
	case BINARY:
		printf("%s %d\n", b->opr, precedence);
	}
}

Constant Consts[] = {
	{ "pi", 3.1415926535897932384626433832795 },
	{ "e", 2.71828182845904523536028747135266 }
};

Node::Node(Node *parent){ this->parent = parent; }

UNode::UNode(Node *parent, UnOp *opr) : Node(parent) {
	r = NULL;
	fn = opr->fn;
}

inline double UNode::value(){
	return (*fn)(r->value());
}

UNode::~UNode(){ if(r != NULL) delete r; }

BNode::BNode(Node *parent, BinOp *opr) : Node(parent) {
	l = r = NULL;
	fn = opr->fn;
}

double BNode::value(){ return (*fn)(l->value(), r->value()); }

BNode::~BNode(){
	if(l != NULL) delete l;
	if(r != NULL) delete r;
}

VarNode::VarNode(Node *parent, double *xptr):Node(parent){
	this->xptr = xptr;
}

inline double VarNode::value(){
	return *xptr;
}

ConstNode::ConstNode(Node *parent, double& val) : Node(parent){
	this->val = val;
}

inline double ConstNode::value(){
	return val;
}

char *errMsg[] = {
	"Out of memory",
	"Syntax error",
	"Operand expected",
	"Binary operator expected",
	"Unbalanced parantheses",
	"Parser not initialized",
	"Unknown symbol"
};

ParserError::ParserError(int code, int pos, char *expr){
	this->code = code;
	this->pos = pos;
	this->expr = expr;
}

Parser::Parser(){
	root = NULL;
	tokens = NULL;
}

//	return false on end else true
//	no error checking in v1
//	buffer will contain next token
bool Parser::nextToken(char *buffer){
	while(isspace(*nextChar))
		nextChar++;


	if(*nextChar == '\0')		//	Watch out for strchr. For some reason
		return false;			//	'\0' is included as part of the string searched
								//	as per the ANSI specs!!!!!!


	if(strchr("+-/*%^()", *nextChar) != NULL){	
		*buffer++ = *nextChar++;
		*buffer = '\0';
		return true;
	}


	if(isdigit(*nextChar) || *nextChar == '.'){
		while(isdigit(*nextChar))
			*buffer++ = *nextChar++;
		if(*nextChar == '.')
			*buffer++ = *nextChar++;
		while(isdigit(*nextChar))
			*buffer++ = *nextChar++;
		if(toupper(*nextChar) == 'E'){
			*buffer++ = *nextChar++;
			if(*nextChar == '+' || *nextChar == '-')
				*buffer++ = *nextChar++;
			while(isdigit(*nextChar))
				*buffer++ = *nextChar++;
		}

		*buffer = '\0';
		return true;
	}

	if(isalpha(*nextChar)){
		while(isalpha(*nextChar))
			*buffer++ = *nextChar++;
		*buffer = '\0';
		return true;
	}

	throw ParserError(ERR_UNRECOGNIZED, nextChar - expr, expr);
}

void Parser::reset(){
	nextChar = expr;
	if(tokens != NULL){
		free(tokens);
		tokens = NULL;
	}
	if(root != NULL){
		delete root;
		root = NULL;
	}
}

	/*
		bool parse(char *expr);
			- set up tokens array and call makeTree
			- all error checking is done here
			- no return value, only exceptions (int or ParserError )
	*/
void Parser::parse(char *expr){
	char buf[MAX_TOKEN_LEN + 1];

	reset();

	this->expr = expr;
	nextChar = expr;

	int nTokens = 0;

	while(nextToken(buf)){
		if(*buf != '(' && *buf != ')')
			nTokens++;
	}

	nextChar = expr;

	tokens = (Token *) malloc(sizeof(Token) * nTokens);
	if(tokens == NULL)
		throw ERR_MEMORY;

	bool expectingOperand = true;
	int nParen = 0, i = 0;
	Constant *constPtr;
	while(nextToken(buf)){
		if(expectingOperand){
			if(buf[0] == '+')
				strcpy(buf, "u+");
			else if(buf[0] == '-')
				strcpy(buf, "u-");
		}

		if(buf[0] == '('){
			if(!expectingOperand)
				throw ParserError(ERR_BINARYEXPECTED, nextChar - expr - strlen(buf), expr);
			nParen++;
			continue;
		}
		else if(buf[0] == ')'){
			if(expectingOperand)
				throw ParserError(ERR_OPERANDEXPECTED, nextChar - expr - strlen(buf), expr);
			if(nParen == 0)
				throw ParserError(ERR_PAREN, nextChar - expr - strlen(buf), expr);
			nParen--;
			continue;
		}
		else if(isdigit(buf[0]) || buf[0] == '.'){
			if(!expectingOperand)
				throw ParserError(ERR_BINARYEXPECTED, nextChar - expr - strlen(buf), expr);
			tokens[i].type = CONSTANT;
			tokens[i].val = atof(buf);
			expectingOperand = false;
		}
		else if((tokens[i].u = getUnOp(buf)) != NULL){
			if(!expectingOperand)
				throw ParserError(ERR_BINARYEXPECTED, nextChar - expr - strlen(buf), expr);
			tokens[i].type = UNARY;
			tokens[i].precedence = tokens[i].u->precedence + nParen * 5;
		}
		else if((tokens[i].b = getBinOp(buf)) != NULL){
			if(expectingOperand)
				throw ParserError(ERR_OPERANDEXPECTED, nextChar - expr - strlen(buf), expr);
			tokens[i].type = BINARY;
			tokens[i].precedence = tokens[i].b->precedence + nParen * 5;
			expectingOperand = true;
		}
		else if(strcmp(buf, "x") == 0){
			if(!expectingOperand)
				throw ParserError(ERR_BINARYEXPECTED, nextChar - expr - strlen(buf), expr);
			tokens[i].type = VARIABLE;
			tokens[i].xptr = &x;
			expectingOperand = false;
		}
		else if((constPtr = getConstant(buf)) != NULL){
			if(!expectingOperand)
				throw ParserError(ERR_BINARYEXPECTED, nextChar - expr - strlen(buf), expr);
			tokens[i].type = CONSTANT;
			tokens[i].val = constPtr->value;
			expectingOperand = false;
		}
		else
			throw ParserError(ERR_UNRECOGNIZED, nextChar - expr - strlen(buf), expr);

		i++;
	}

	if(nParen != 0)
		throw ParserError(ERR_PAREN, nextChar - expr - strlen(buf), expr);
	if(expectingOperand)
		throw ParserError(ERR_OPERANDEXPECTED, nextChar - expr - strlen(buf), expr);
	root = makeTree(NULL, 0, i - 1);

	if(root == NULL)
		throw ERR_MEMORY;
}

/*
	Node *makeTree(Node *parent, int start, int end)
		Recursively sets up tree based on the parsed expression
		parent - parent node
		start, end - indexes into tokens array indicating the bounds
			starting and ending tokens to consider

*/
Node *Parser::makeTree(Node *parent, int start, int end){
	int i, split = -1;

	if(start == end){
		if(tokens[start].type == CONSTANT)
			return new ConstNode(parent, tokens[start].val);
		else if(tokens[start].type == VARIABLE)
			return new VarNode(parent, &x);
	}

	//_ASSERTE(start != end);

	for(i = end; i >= start; i--){
		if(tokens[i].type == UNARY || tokens[i].type == BINARY)
			if(split == -1 || tokens[i].precedence < tokens[split].precedence)
				split = i;
	}

	//_ASSERTE(tokens[split].type == UNARY || tokens[split].type == BINARY);

	Node *thisNode;
	if(tokens[split].type == UNARY){
		UNode *unode = new UNode(parent, tokens[split].u);
		if(unode == NULL) return NULL;
		unode->r = makeTree(unode, split + 1, end);
		if(unode->r == NULL){ delete unode; return NULL; }
		thisNode = unode;
	}
	else {
		BNode *bnode = new BNode(parent, tokens[split].b);
		if(bnode == NULL) return NULL;
		bnode->l = makeTree(bnode, start, split - 1);
		if(bnode->l == NULL){ delete bnode; return NULL; }
		bnode->r = makeTree(bnode, split + 1, end);
		if(bnode->r == NULL){ delete bnode; return NULL; }

		thisNode = bnode;
	}

	return thisNode;
}

UnOp *Parser::getUnOp(char *op){
	int i, n;

	n = sizeof(UnOps) / sizeof(UnOps[0]);
	for(i = 0; i < n; i++)
		if(strcmp(UnOps[i].opr, op) == 0)
			return UnOps + i;
	return NULL;
}

BinOp *Parser::getBinOp(char *op){
	int i, n;

	n = sizeof(BinOps) / sizeof(BinOps[0]);
	for(i = 0; i < n; i++)
		if(strcmp(BinOps[i].opr, op) == 0)
			return BinOps + i;
	return NULL;
}

Constant *Parser::getConstant(char *name){
	int i, n = sizeof(Consts) / sizeof(Consts[0]);

	for(i = 0; i < n; i++)
		if(strcmp(Consts[i].name, name) == 0)
			return &Consts[i];
	return NULL;
}

bool Parser::isParsed(){
	if(root != NULL)
		return true;
	else
		return false;
}

double Parser::evaluate(double x){
	this->x = x;
	if(root == NULL)
		throw ERR_NOTINITIALIZED;
	return root->value();
}

//	replace unary operator function
void Parser::replaceUnOpFn(char *name, double (*fn)(double)){
	int i, n;
	n = sizeof(UnOps) / sizeof(UnOps[0]);
	for(i = 0; i < n; i ++)
		if(strcmp(UnOps[i].opr, name) == 0){
			UnOps[i].fn = fn;
			return;
		}
}

void Parser::setTrigMode(TrigMode tm){
	if(tm == RADIANS){
		replaceUnOpFn("sin", sin);
		replaceUnOpFn("cos", cos);
		replaceUnOpFn("tan", tan);
		replaceUnOpFn("asin", asin);
		replaceUnOpFn("acos", acos);
		replaceUnOpFn("atan", atan);
	}
	else if(tm == DEGREES){
		replaceUnOpFn("sin", sind);
		replaceUnOpFn("cos", cosd);
		replaceUnOpFn("tan", tand);
		replaceUnOpFn("asin", asind);
		replaceUnOpFn("acos", acosd);
		replaceUnOpFn("atan", atand);
	}
	else if(tm == GRADES){
		replaceUnOpFn("sin", sing);
		replaceUnOpFn("cos", cosg);
		replaceUnOpFn("tan", tang);
		replaceUnOpFn("asin", asing);
		replaceUnOpFn("acos", acosg);
		replaceUnOpFn("atan", atang);
	}
}

char *Parser::getExpr(){
	return expr;
}

Parser::~Parser(){
	if(tokens != NULL)
		free(tokens);

	if(root != NULL)
		delete root;
}


