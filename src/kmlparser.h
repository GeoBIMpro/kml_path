#ifndef __KMLPARSER_H__
#define __KMLPARSER_H__

#include <libxml/tree.h>
#include <libxml/parser.h>

#include "wgs84.h"

#ifdef __cplusplus
extern "C"
{
#endif

xmlDocPtr kmlReadFile( const char *filename );
void kmlFree( xmlDocPtr doc );
int kmlGetLines( xmlNode *root_element, blh_t ***lines );
int kmlGetPolygon( xmlNode *node, blh_t **lines, int nline );
int kmlGetPolygons( xmlNode *root_element, blh_t ***lines );
void kmlFreeLines( blh_t ***lines, int nlines );
int kmlGetPointCount( blh_t *lines );
int kmlGetFolders( xmlDocPtr doc, xmlNode ***xmlnodes );
void kmlFreeFolders( xmlNode ***xmlnodes );
int kmlGetFolderName( xmlNode *node, char *name );
int kmlGetPoints( xmlNode *root_element, blh_t ***lines, char ***name );
int kmlGetImage( xmlNode *node, blh_t *rect, char *name );

#ifdef __cplusplus
}
#endif

#endif

