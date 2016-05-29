#ifndef __WGS84_H__
#define __WGS84_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __cplusplus
extern "C"
{
#endif

// WGS-84のパラメータを使用

// 地球の長半径
#define SEMIMAJOR_AXIS	( 6378137.0 )
// 地球の短半径
#define SEMIMINOR_AXIS	( SEMIMAJOR_AXIS * ( 1.0 - OBLATENESS ) )
// 偏平率の逆数
#define INV_OBLATENESS	( 298.257223563 )
// 偏平率
#define OBLATENESS		( 1.0 / ( INV_OBLATENESS ) )
// 長半径方向の離心率の二乗
#define ECC_MAJ_SQ		( ( pow( SEMIMAJOR_AXIS, 2 ) - pow( SEMIMINOR_AXIS, 2 ) ) / pow( SEMIMAJOR_AXIS, 2 ) )
// 楕円体の緯度に対する半径
#define EC_RADIUS(lat)	( ( SEMIMAJOR_AXIS ) / sqrt( 1.0 - ( ECC_MAJ_SQ ) * pow( sin( ( lat ) ), 2 ) ) )
// 度ラジアン変換
#define deg2rad( theta )	( ( theta ) * M_PI / 180.0 )

// 短半径方向の離心率の二乗
//#define ECC_MIN_SQ	( ECC_SQ * pow( SEMIMAJOR_AXIS, 2 ) / ( pow( SEMIMINOR_AXIS, 2 ) ) )
// 光速
//#define C			2.99792458E+08
// 重力定数
//#define GRAV_CONST	3.986005E+14
// 地球の平均角速度
//#define EARTH_ROTATION	7.2921151467E-05
// 定数
//#define REL_CONST		4.442807633E-10

#define dist(a,b)		( sqrt( pow( (a).x - (b).x, 2 ) + pow( (a).y - (b).y, 2 ) + pow( (a).z - (b).z, 2 ) ) )
#define dot(a,b)		( (a).x * (b).x + (a).y * (b).y + (a).z * (b).z )
#define cross(a,b)		( sqrt( dot( a, a ) * dot( b, b ) - pow( dot( a, b ), 2 ) ) )
#define sub(a,b,c)		{ (c).x = (a).x - (b).x; (c).y = (a).y - (b).y; (c).z = (a).z - (b).z; }
#define normal(a)		( sqrt( dot( a, a ) ) )
#define cross2d(a,b)	( (a).x * (b).y - (a).y * (b).x )

typedef struct
{
	double x;
	double y;
	double z;
} ecef_t;

typedef struct
{
	double x;
	double y;
	double z;
} enu_t;

typedef struct
{
	double lat;
	double lon;
	double height;
} blh_t;


ecef_t * blh2ecef( blh_t * blh, ecef_t * ecef, int n );
enu_t * ecef2enu( blh_t * origin_blh, ecef_t * ecef, enu_t * enu, int n );
enu_t * blh2enu( blh_t * origin_blh, blh_t * blh, enu_t * enu, int n );
enu_t *enu_linear( enu_t *pos, enu_t *pos1, enu_t *pos2, float ratio );

#ifdef __cplusplus
}
#endif

#endif

