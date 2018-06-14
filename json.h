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

typedef const char *StringPtr;

int build_dom( struct Node* *root, FILE *input_file );
void free_node( struct Node *node );
bool strings_equal( StringPtr str1, StringPtr str2 );