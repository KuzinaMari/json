#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"

void print_node( char indent[STRMAX], struct Node *node ){
	printf( "%s%s", indent, node->type );
	if( !strings_equal( node->name, "" ) ) printf( ", %s:", node->name );
	if( !strings_equal( node->value, "" ) ) printf( " %s", node->value );
	if( node->children_count >0 ) printf( ", children: %d", node->children_count );
	printf( "\n" );
	char indent2[STRMAX];
	strncpy( indent2, indent, STRMAX );
	strcat( indent2, "	" );
	for( int i =0; i < node->children_count; i++ ){
		print_node( indent2, node->children[ i ] );
	}
}

void print_input( struct State *machine ){
	machine->input_file = fopen("input.json", "r"); // read mode
	while((machine->ch = fgetc(machine->input_file)) != EOF){
			printf("%c", machine->ch);
	}
	fclose(machine->input_file);
}