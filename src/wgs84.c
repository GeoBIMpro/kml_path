#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "wgs84.h"


ecef_t * blh2ecef( blh_t * blh, ecef_t * ecef, int n )
{
	int i;

	for( i = 0; i < n; i ++ )
	{
		double lat;
		double lon;
		double height;
		double radius;

		lat = deg2rad( blh[i].lat );
		lon = deg2rad( blh[i].lon );
		height = blh[i].height;
		radius = EC_RADIUS( lat );

		ecef[i].x = ( radius + height ) * cos( lat ) * cos( lon );
		ecef[i].y = ( radius + height ) * cos( lat ) * sin( lon );
		ecef[i].z = ( radius * ( pow( SEMIMAJOR_AXIS, 2 ) / pow( SEMIMINOR_AXIS, 2 ) ) + height ) * sin( lat );
	}
	return ecef;
}


enu_t * ecef2enu( blh_t *origin_blh, ecef_t * ecef, enu_t * enu, int n )
{
/*
変換行列は
matrix(
	[-sin(lon),          cos(lon),          0        ],
	[-sin(lat)*cos(lon),-sin(lat)*sin(lon), cos(lat) ],
	[ cos(lat)*cos(lon), cos(lat)*sin(lon), sin(lat) ]
);
*/
	int i;
	double lat;
	double lon;
	double mat[3][3];
	ecef_t origin;

	lat = deg2rad( origin_blh->lat );
	lon = deg2rad( origin_blh->lon );

	mat[0][0] = -sin(lon);
	mat[0][1] =  cos(lon);
	mat[0][2] =  0;
	mat[1][0] = -sin(lat)*cos(lon);
	mat[1][1] = -sin(lat)*sin(lon);
	mat[1][2] =  cos(lat);
	mat[2][0] =  cos(lat)*cos(lon);
	mat[2][1] =  cos(lat)*sin(lon);
	mat[2][2] =  sin(lat);

	blh2ecef( origin_blh, &origin, 1 );

	for( i = 0; i < n; i ++ )
	{
		double x, y, z;

		x = ecef[i].x - origin.x;
		y = ecef[i].y - origin.y;
		z = ecef[i].z - origin.z;

		enu[i].x = mat[0][0] * x + mat[0][1] * y;// + mat[0][2] * z;
		enu[i].y = mat[1][0] * x + mat[1][1] * y + mat[1][2] * z;
		enu[i].z = mat[2][0] * x + mat[2][1] * y + mat[2][2] * z;
	}
	return enu;
}


enu_t * blh2enu( blh_t *origin_blh, blh_t * blh, enu_t * enu, int n )
{
	ecef_t *ecef;

	if( enu == NULL ) enu = (enu_t*)malloc( sizeof(enu_t) * n );
	
	ecef = (ecef_t*)malloc( sizeof(ecef_t) * n );
	if( ecef == NULL ) return NULL;

	ecef2enu( origin_blh, blh2ecef( blh, ecef, n ), enu, n );
	free( ecef );

	return enu;
}

enu_t *enu_linear( enu_t *pos, enu_t *pos1, enu_t *pos2, float ratio )
{
	pos->x = ( ( 1.0 - ratio ) * pos1->x + ratio * pos2->x );
	pos->y = ( ( 1.0 - ratio ) * pos1->y + ratio * pos2->y );
	pos->z = ( ( 1.0 - ratio ) * pos1->z + ratio * pos2->z );

	return pos;
}


