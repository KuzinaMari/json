#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"

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
void print_self_node( struct Node *node );

void print_input( struct State *machine );
int process( struct State *machine );
int process_char( struct State *machine );

typedef void (*StateHandler)(struct State *);
typedef char String[STRMAX];
typedef struct State *Machine;
typedef struct Node *NodePtr;

void init_state( struct State *machine, FILE *input_file );

// int main(){

// 	struct State *machine;
// 	machine =(struct State *) malloc (sizeof(struct State));

// 	print_input( machine );

// 	printf("\n\n");

// 	init_state( machine, fopen("input.json", "r") );
// 	process( machine );
// 	end_state( machine );

// 	return 0;
// }

int build_dom( struct Node* *root, FILE *input_file ){
	struct State *machine;
	machine =(struct State *) malloc (sizeof(struct State));
	init_state( machine, input_file );
	if( process( machine ) ){
		free( machine );
		return 1;
	}
	*root =machine->root;
	free( machine );
	return 0;
}

bool strings_equal( StringPtr str1, StringPtr str2 ){
	return strcmp( str1, str2 ) ==0; }

void free_node( struct Node *node ){ 
	for( int i =0; i < node->children_count; i++ ){
		free_node( node->children[ i ] );
	}
	free( node );
}

void init_state( struct State *machine, FILE *input_file ){
	machine->root =0;
	machine->input_file = input_file; // read mode

	strncpy( machine->name, "", STRMAX );
	strncpy( machine->key, "", STRMAX );
	strncpy( machine->token, "", STRMAX );
	strncpy( machine->last_key, "", STRMAX );
	machine->token_length =0;

	machine->current =0;
	machine->parent =0;
	machine->escape =false;
}

int process( struct State *machine ){
	while((machine->ch = fgetc(machine->input_file)) != EOF){
		//printf("%c\n", ch);
		if( process_char( machine ) ) return 1;
	}
	return 0;
}

bool check_state( Machine machine, StringPtr state ){
	return strings_equal( machine->name, state ); }

void string_copy( char *dest, StringPtr src ){
	strncpy( dest, src, STRMAX ); }

bool check_char( Machine machine, char ch ){
	return machine->ch ==ch; }

bool is_number_char( Machine machine ){
	return machine->ch =='.' || ( machine->ch >= '0' && machine->ch <= '9' ); }

void set_type( Machine machine, StringPtr type ){
	string_copy( machine->current->type, type ); }

bool is_token_state( StringPtr state ){
	return strings_equal( state, "string" ) || strings_equal( state, "number" ); }

void set_key( NodePtr node, StringPtr key ){
	string_copy( node->name, key ); }

void empty_token( Machine machine ){
	string_copy( machine->token, "" );
	machine->token_length =-1; }


int new_node( Machine machine, StringPtr type ){
	if( machine->parent !=0 ){
		if( machine->parent->children_count >= MAXCHILDREN ){
			return 1;
		}
	}
	machine->current=(NodePtr) malloc (sizeof(struct Node));
	machine->current->children_count =0;
	string_copy( machine->current->name, "" );
	string_copy( machine->current->value, "" );
	set_type( machine, type );
	if( machine->parent !=0 ){
		machine->parent->children[ machine->parent->children_count ] =machine->current;
		machine->parent->children_count++;
		machine->current->parent =machine->parent;
		set_key( machine->current, machine->key );
	}else{
		machine->parent = machine->current;
		machine->root = machine->current;
	}
	string_copy( machine->key, "" );
	return 0;
}

int state_change_handler( Machine machine, StringPtr old_state, StringPtr new_state ){
	if( machine->parent != 0 ){
		if( strings_equal( machine->parent->type, "list" )
			&& is_token_state( old_state ) ){
				string_copy( machine->key, "" );
				if( new_node( machine, old_state ) ) return 1; }
		if( strings_equal( machine->parent->type, "hash" )
			&& strings_equal( old_state, "string" ) ){
				string_copy( machine->key, machine->token ); }
		if( is_token_state( new_state ) ){
			if( machine->current != 0 ) set_type( machine, new_state ); } }
	if( machine->current != 0 ){
		if( is_token_state( old_state ) ){
			if( strings_equal( machine->current->type, "string" )
				|| strings_equal( machine->current->type, "number" ) ){
				string_copy( machine->current->value, machine->token );
				 } } }
	empty_token( machine );
	if( strings_equal( new_state, "number" ) ){
		machine->token[0] =machine->ch;
		machine->token[1] =0;
		machine->token_length =1; }
	if( strings_equal( old_state, "number" ) ){
		if( process_char( machine ) ) return 1; }
	return 0; }

int set_state( Machine machine, StringPtr state ){
	String old_state ="";
	string_copy( old_state, machine->name );
	if( !check_state( machine, state ) ){
		string_copy( machine->name, state );
		if( state_change_handler( machine, old_state, state ) ) return 1; }
	return 0; }

int append_char( Machine machine ){
	if( machine->token_length >=STRMAX ) return 1;
	if( machine->token_length <0 ) machine->token_length =0;
	machine->token[ machine->token_length ] =machine->ch;
	machine->token[ machine->token_length +1 ] =0;
	machine->token_length++;
	return 0; }

int process_char( Machine machine ){
	if( machine->ch == '\n' || machine->ch == ' ' )
		return 0;

	if( check_state( machine, "string" ) ){
		if( machine->escape ){
			if( append_char( machine ) ) return 1;
			machine->escape =false; }
		else if( check_char( machine, '\\' ) ){
			machine->escape =true; }
		else if( check_char( machine, '"' ) ){
			if( set_state( machine, "" ) ) return 1; }
		else{
			if( append_char( machine ) ) return 1; } }
	else if( check_state( machine, "number" ) ){
		if( is_number_char( machine ) ){
			if( append_char( machine ) ) return 1; }
		else{
			if( set_state( machine, "" ) ) return 1; } }
	else{
		if( check_char( machine, '{' ) || check_char( machine, '[' ) ){
			String type ="hash";
			if( check_char( machine, '[' ) ) string_copy( type, "list" );
			if( machine->root ==0 ){
				if( new_node( machine, type ) ) return 1; }
			else{
				if( strings_equal( machine->parent->type, "list" ) ){
					if( new_node( machine, type ) ) return 1; }
				else{
					set_type( machine, type );
				 }
				machine->parent =machine->current; }
			machine->current =0; }
		else if( check_char( machine, '"' ) ){
			if( set_state( machine, "string" ) ) return 1; }
		else if( is_number_char( machine ) ){
			if( set_state( machine, "number" ) ) return 1; }
		else if( check_char( machine, ':' ) ){
			if( new_node( machine, "" ) ) return 1; }
		else if( check_char( machine, ',' ) ){
			machine->current =0;
			if( !strings_equal( machine->parent->type, "list" ) ){
				machine->current =0; } }
		else if( check_char( machine, '}' ) || check_char( machine, ']' ) ){
			if( strings_equal( machine->parent->type, "list" )
				 && machine->token_length >= 0 ){
				 }
			machine->parent =machine->parent->parent;
			machine->current =0; }
	 }
	strncpy( machine->previous, machine->name, STRMAX );
	return 0;
}