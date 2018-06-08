#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"
#include "json_print.h"

int main(){

	struct State *machine;
	machine =(struct State *) malloc (sizeof(struct State));

	print_input( machine );

	printf("\n\n");

	init_state( machine );
	process( machine );
	end_state( machine );

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

void init_state( struct State *machine ){
	machine->input_file = fopen("input.json", "r"); // read mode

	strncpy( machine->name, "", STRMAX );
	strncpy( machine->key, "", STRMAX );
	strncpy( machine->token, "", STRMAX );
	strncpy( machine->last_key, "", STRMAX );
	machine->token_length =0;

	machine->current =0;
	machine->parent =0;
	machine->escape =false;
}

void end_state( struct State *machine ){
	char indent[STRMAX] ="";
	print_node( indent, machine->root );
 
	free_node( machine->root );
	fclose(machine->input_file);
	free( machine );
}

void process( struct State *machine ){
	while((machine->ch = fgetc(machine->input_file)) != EOF){
		//printf("%c\n", ch);
		process_char( machine );
	}
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


void new_node( Machine machine, StringPtr type ){
	machine->current=(NodePtr) malloc (sizeof(struct Node));
	machine->current->children_count =0;
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
}

void state_change_handler( Machine machine, StringPtr old_state, StringPtr new_state ){
	if( machine->parent != 0 ){
		if( strings_equal( machine->parent->type, "list" )
			&& is_token_state( old_state ) ){
				string_copy( machine->key, "" );
				new_node( machine, old_state ); }
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
		process_char( machine ); } }

void set_state( Machine machine, StringPtr state ){
	String old_state ="";
	string_copy( old_state, machine->name );
	if( !check_state( machine, state ) ){
		string_copy( machine->name, state );
		state_change_handler( machine, old_state, state ); } }

void append_char( Machine machine ){
	if( machine->token_length <0 ) machine->token_length =0;
	machine->token[ machine->token_length ] =machine->ch;
	machine->token[ machine->token_length +1 ] =0;
	machine->token_length++; }

void process_char( Machine machine ){
	if( machine->ch == '\n' || machine->ch == ' ' )
		return;

	if( check_state( machine, "string" ) ){
		if( machine->escape ){
			append_char( machine );
			machine->escape =false; }
		else if( check_char( machine, '\\' ) ){
			machine->escape =true; }
		else if( check_char( machine, '"' ) ){
			set_state( machine, "" ); }
		else{
			append_char( machine ); } }
	else if( check_state( machine, "number" ) ){
		if( is_number_char( machine ) ){
			append_char( machine ); }
		else{
			set_state( machine, "" ); } }
	else{
		if( check_char( machine, '{' ) || check_char( machine, '[' ) ){
			String type ="hash";
			if( check_char( machine, '[' ) ) string_copy( type, "list" );
			if( machine->root ==0 ){
				new_node( machine, type ); }
			else{
				if( strings_equal( machine->parent->type, "list" ) ){
					new_node( machine, type ); }
				else{
					set_type( machine, type );
				 }
				machine->parent =machine->current; }
			machine->current =0; }
		else if( check_char( machine, '"' ) ){
			set_state( machine, "string" ); }
		else if( is_number_char( machine ) ){
			set_state( machine, "number" ); }
		else if( check_char( machine, ':' ) ){
			new_node( machine, "" ); }
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
}