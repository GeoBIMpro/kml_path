#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <libxml/tree.h>
#include <libxml/parser.h>

#include "wgs84.h"


xmlDocPtr kmlReadFile( const char *filename )
{
    return xmlReadFile( filename, NULL, 0 );
}

void kmlFree( xmlDocPtr doc )
{
    xmlFreeDoc( doc );
}

int kmlGetLineData( xmlNode *node, blh_t **lines, int nline )
{
	xmlNode *cur_node = NULL;
	int npoints = 0;

	for( cur_node = node; cur_node; cur_node = cur_node->next )
	{
		if( cur_node->type == XML_ELEMENT_NODE )
		{
			if( strcmp( (char*)cur_node->name, "coordinates" ) == 0 )
			{
				if( lines )
				{
					char *points;
					char *saveptr;
					int npoints;

					npoints = 0;
					points = (char*)xmlNodeGetContent( cur_node );
					saveptr = NULL;
					for( points = strtok_r( points, " \t", &saveptr );
						 points;
						 points = strtok_r( NULL, " \t", &saveptr ) )
					{
						double dummy;
						if( sscanf( points, "%lf,%lf,%lf", &dummy, 
														&dummy, 
														&dummy ) == 3 )
						{
							npoints ++;
						}
					}
					lines[ nline ] = malloc( sizeof( blh_t ) * ( npoints + 1 ) );

					npoints = 0;
					points = (char*)xmlNodeGetContent( cur_node );
					saveptr = NULL;
					for( points = strtok_r( points, " \t", &saveptr );
						 points;
						 points = strtok_r( NULL, " \t", &saveptr ) )
					{
						if( sscanf( points, "%lf,%lf,%lf", &lines[ nline ][ npoints ].lon, 
														&lines[ nline ][ npoints ].lat, 
														&lines[ nline ][ npoints ].height ) == 3 )
						{
							npoints ++;
						}
					}
					lines[ nline ][ npoints ].lat = 0;
					lines[ nline ][ npoints ].lon = 0;
					lines[ nline ][ npoints ].height = 0;

					return npoints;
				}
			}
		}

		npoints = kmlGetLineData( cur_node->children, lines, nline );
		if( npoints ) break;
	}
	return npoints;
}

int kmlGetLine( xmlNode *node, blh_t **lines, int nline )
{
	xmlNode *cur_node = NULL;

	for( cur_node = node; cur_node; cur_node = cur_node->next )
	{
		if( cur_node->type == XML_ELEMENT_NODE )
		{
			if( strcmp( (char*)cur_node->name, "LineString" ) == 0 )
			{
				if( lines )
				{
					kmlGetLineData( cur_node, lines, nline );
				}
				nline ++;
			}
		}

		nline = kmlGetLine( cur_node->children, lines, nline );
	}
	return nline;
}

int kmlGetLines( xmlNode *root_element, blh_t ***lines )
{
	int nlines;

	nlines = kmlGetLine( root_element, NULL, 0 );

	*lines = malloc( sizeof( blh_t * ) * nlines );
	nlines = kmlGetLine( root_element, *lines, 0 );

	return nlines;
}

int kmlGetPolygon( xmlNode *node, blh_t **lines, int nline )
{
	xmlNode *cur_node = NULL;

	for( cur_node = node; cur_node; cur_node = cur_node->next )
	{
		if( cur_node->type == XML_ELEMENT_NODE )
		{
			if( strcmp( (char*)cur_node->name, "Polygon" ) == 0 )
			{
				if( lines )
				{
					kmlGetLineData( cur_node, lines, nline );
				}
				nline ++;
			}
		}

		nline = kmlGetPolygon( cur_node->children, lines, nline );
	}
	return nline;
}

int kmlGetPolygons( xmlNode *root_element, blh_t ***lines )
{
	int nlines;

	nlines = kmlGetPolygon( root_element, NULL, 0 );

	*lines = malloc( sizeof( blh_t * ) * nlines );
	nlines = kmlGetPolygon( root_element, *lines, 0 );

	return nlines;
}

int kmlGetPoint( xmlNode *node, blh_t **lines, char **name, int nline )
{
	xmlNode *cur_node = NULL;

	for( cur_node = node; cur_node; cur_node = cur_node->next )
	{
		if( cur_node->type == XML_ELEMENT_NODE )
		{
			if( strcmp( (char*)cur_node->name, "Point" ) == 0 )
			{
				if( lines )
				{
					kmlGetLineData( cur_node, lines, nline );

					xmlNode *name_node = NULL;

					for( name_node = cur_node->parent->children; name_node; name_node = name_node->next )
					{
						if( name_node->type == XML_ELEMENT_NODE )
						{
							if( strcmp( (char*)name_node->name, "name" ) == 0 ) break;
						}
					}
					if( name_node )
					{
						name[nline] = malloc( sizeof(char) * ( strlen((char*)xmlNodeGetContent( name_node )) + 1 ) );
						strcpy( name[nline], (char*)xmlNodeGetContent( name_node ) );
					}
					else
					{
						name[nline] = NULL;
					}
				}
				nline ++;
			}
		}

		nline = kmlGetPoint( cur_node->children, lines, name, nline );
	}
	return nline;
}

int kmlGetPoints( xmlNode *root_element, blh_t ***lines, char ***name )
{
	int nlines;

	nlines = kmlGetPoint( root_element, NULL, *name, 0 );

	*lines = malloc( sizeof( blh_t * ) * nlines );
	*name = malloc( sizeof( char * ) * nlines );
	nlines = kmlGetPoint( root_element, *lines, *name, 0 );

	return nlines;
}

int kmlGetPointCount( blh_t *lines )
{
	blh_t *point_blh;
	int npoints;

	npoints = 0;
	for( point_blh = lines; 
	     point_blh->lat != 0 || point_blh->lon != 0 || point_blh->height != 0; 
	     point_blh ++ )
	{
		npoints ++;
	}

	return npoints;
}


int kmlGetText( xmlNode *node, char *name, char *tag )
{
	xmlNode *cur_node = NULL;

	for( cur_node = node; cur_node; cur_node = cur_node->next )
	{
		if( cur_node->type == XML_ELEMENT_NODE )
		{
			if( strcmp( (char*)cur_node->name, tag ) == 0 )
			{
				strcpy( name, (char*)xmlNodeGetContent( cur_node ) );
				return 1;
			}
			if( kmlGetText( cur_node->children, name, tag ) ) return 1;
		}
	}
	return 0;
}

int kmlGetImage( xmlNode *node, blh_t *rect, char *name )
{
	xmlNode *cur_node = NULL;

	for( cur_node = node; cur_node; cur_node = cur_node->next )
	{
		if( cur_node->type == XML_ELEMENT_NODE )
		{
			if( strcmp( (char*)cur_node->name, "GroundOverlay" ) == 0 )
			{	
				char value[512];
				kmlGetText( cur_node->children, name, "href" );
				kmlGetText( cur_node->children, value, "north" );
				rect[0].lat = atof( value );
				kmlGetText( cur_node->children, value, "east" );
				rect[0].lon = atof( value );
				kmlGetText( cur_node->children, value, "south" );
				rect[1].lat = atof( value );
				kmlGetText( cur_node->children, value, "west" );
				rect[1].lon = atof( value );
				return 1;
			}
		}
		if( kmlGetImage( cur_node->children, rect, name ) == 1 ) return 1;
	}
	return 0;
}

void kmlFreeLines( blh_t ***lines, int nlines )
{
	int i;

	for( i = 0; i < nlines; i ++ )
	{
		free( (*lines)[i] );
	}
	free( *lines );
}

int kmlGetFolder( xmlNode *node, xmlNode **xmlnodes, int nfolders )
{
	xmlNode *cur_node = NULL;

	for( cur_node = node; cur_node; cur_node = cur_node->next )
	{
		if( cur_node->type == XML_ELEMENT_NODE )
		{
			if( strcmp( (char*)cur_node->name, "Folder" ) == 0 )
			{
				if( xmlnodes )
				{
					xmlnodes[ nfolders ] = cur_node->children;
				}
				nfolders ++;
			}
		}

		nfolders = kmlGetFolder( cur_node->children, xmlnodes, nfolders );
	}
	return nfolders;
}

int kmlGetFolderName( xmlNode *node, char *name )
{
	xmlNode *cur_node = NULL;

	name[0] = 0;
	for( cur_node = node; cur_node; cur_node = cur_node->next )
	{
		if( cur_node->type == XML_ELEMENT_NODE )
		{
			if( strcmp( (char*)cur_node->name, "name" ) == 0 )
			{
				strcpy( name, (char*)xmlNodeGetContent( cur_node ) );
				return strlen( name );
				break;
			}
		}
	}
	return 0;
}
int kmlGetFolders( xmlDocPtr doc, xmlNode ***xmlnodes )
{
	xmlNode *root_element = NULL;
	int nfolders;

	root_element = xmlDocGetRootElement( doc );
	nfolders = kmlGetFolder( root_element, NULL, 0 );

	*xmlnodes = malloc( sizeof( xmlNode * ) * nfolders );
	nfolders = kmlGetFolder( root_element, *xmlnodes, 0 );

	return nfolders;
}

void kmlFreeFolders( xmlNode ***xmlnodes )
{
	free( *xmlnodes );
}

