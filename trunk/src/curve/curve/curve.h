#ifndef _CURVE_H_
#define _CURVE_H_

/*
	To do:
		- User definable variables
*/
#define MAX_TOKEN_LEN 100

enum { CONSTANT, VARIABLE, UNARY, BINARY };	//	token type

double uplus(double val);
double uminus(double val);

const double PI  = 3.1415926535897932384626433832795;

#define D2R(X) (X * PI / 180.0)
#define R2D(X) (X * 180.0 / PI)

#define G2R(X) (X * PI / 200.0)
#define R2G(X) (X * 200.0 / PI)

double sind(double deg);
double cosd(double deg);
double tand(double deg);

double asind(double x);
double acosd(double x);
double atand(double x);

struct UnOp{
	char *opr;
	double (*fn)(double val);
	int precedence;
};

extern UnOp UnOps[];

double add(double a, double b);
double sub(double a, double b);
double div(double a, double b);
double mul(double a, double b);

struct BinOp {
	char *opr;
	double (*fn)(double a, double b);
	int precedence;
};

extern BinOp BinOps[];

struct Token {
	unsigned char type;
	unsigned char precedence;
	union {
		BinOp *b;
		UnOp *u;
		double val;		//	constants
		double *xptr;	//	x
	};

	void dump();

};

struct Constant {
	char *name;
	double value;
};

extern Constant Consts[];

class Node {
	Node *parent;
public:
	Node(Node *parent);
	virtual double value() = 0;
	virtual ~Node(){ }
};

class UNode : public Node {
	double (*fn)(double);
public:
	Node *r;
	UNode(Node *parent, UnOp *opr);
	inline double value();;

	~UNode();
};

struct BNode : public Node {
	double (*fn)(double, double);
public:
	Node *l;
	Node *r;
	BNode(Node *parent, BinOp *opr);
	inline double value();
	~BNode();
};

class VarNode : public Node {
	double *xptr;
public:
	VarNode(Node *parent, double *xptr);
	inline double value();
};

class ConstNode : public Node {
	double val;
public:
	ConstNode(Node *parent, double& val);
	inline double value();
};

enum { ERR_MEMORY, ERR_SYNTAX, ERR_OPERANDEXPECTED, ERR_BINARYEXPECTED,
	ERR_PAREN, ERR_NOTINITIALIZED, ERR_UNRECOGNIZED };

extern char *errMsg[];

struct ParserError {	//	only thrown in case of parse errors
	int code;			//	other errors are indicated by throwing ints
	int pos;
	char *expr;

	ParserError(int code, int pos, char *expr);	
};

enum TrigMode { RADIANS, DEGREES, GRADES };

class Parser {
	char *expr;
	char *nextChar;
	Token *tokens;
	double x;
	Node *root;


	/*
		Node *makeTree(Node *parent, int start, int end)
			Recursively sets up tree based on the parsed expression
			parent - parent node
			start, end - indexes into tokens array indicating the bounds
				starting and ending tokens to consider

	*/
	Node *makeTree(Node *parent, int start, int end);

	/* getUnOp - get unary operator from string representation */
	UnOp *getUnOp(char *op);

	/* get binary operator from string representation */
	BinOp *getBinOp(char *op);
	
	/* get pre-defined constant */
	Constant *getConstant(char *name);

	//	replace unary operator handler
	void replaceUnOpFn(char *name, double (*fn)(double));

	//	return false on end else true
	//	no error checking in v1
	//	buffer will contain next token
	bool nextToken(char *buffer);
	void reset();

public:
	Parser();

	/*
		bool parse(char *expr);
			- set up tokens array and call makeTree
			- all error checking is done here
			- no return value, only exceptions (int or ParserError )
	*/
	void parse(char *expr);
	char *getExpr();

	/* calculate value of the expression based on current value of vars */
	double evaluate(double x);
	
	//	set mode for trigonometric functions
	void setTrigMode(TrigMode tm);

	bool isParsed();
	~Parser();
};

#endif