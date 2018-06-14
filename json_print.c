#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"

void print_node( char indent[STRMAX], struct Node *node );
void print_input( FILE *input_file );

int main(int argc, char *argv[]){

	if( argc !=2 ){
		printf("specify input file\n");
		exit(1);
	}
	printf("input file: %s\n", argv[1]);

	FILE *input_file;
	if( ( input_file =fopen(argv[1], "r") ) ==0 ){
		printf( "Cannot open file \n" );
		exit( 1 );
	}
	print_input( input_file );
	fclose( input_file );

	input_file =fopen(argv[1], "r");
	struct Node* root =0;
	if( build_dom( &root, input_file ) ){
		printf( "Incorrect data \n" );
		fclose( input_file );
		exit( 1 );
	}
	char indent[STRMAX] ="";
	print_node( indent, root );
	free_node( root );
	fclose( input_file );

	return 0;
}

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

void print_input( FILE *input_file ){
	char ch;
	while((ch = fgetc(input_file)) != EOF){
			printf("%c", ch);
	}
	printf("\n");
}