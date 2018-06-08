#define STRMAX 100
#define MAXCHILDREN 100

struct Node{
	char type[STRMAX];
	char name[STRMAX];
	char value[STRMAX];
	int children_count;
	struct Node *parent;
	struct Node *children[MAXCHILDREN];
};

struct State{
	char ch;
	FILE *input_file;
	char name[STRMAX];
	char previous[STRMAX];
	char key[STRMAX];
	char token[STRMAX];
	struct Node *current;
	struct Node *parent;
	struct Node *root;
	char last_key[STRMAX];
	int token_length;
	bool escape;
};

void print_node( char indent[STRMAX], struct Node *node );
void free_node( struct Node *node );
void print_self_node( struct Node *node );

void print_input( struct State *machine );
void init_state( struct State *machine );
void end_state( struct State *machine );
void process( struct State *machine );
void process_char( struct State *machine );

typedef void (*StateHandler)(struct State *);
typedef char String[STRMAX];
typedef struct State *Machine;
typedef struct Node *NodePtr;
typedef const char *StringPtr;

bool strings_equal( StringPtr str1, StringPtr str2 );