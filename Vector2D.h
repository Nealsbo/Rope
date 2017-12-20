#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <math.h>

class Vector2D
{
public:
    float x;
    float y;

    Vector2D ();
    Vector2D ( float _x, float _y );

    virtual ~Vector2D ();

    Vector2D  operator+  ( Vector2D v )  { return Vector2D( x + v.x, y + v.y ); }
	Vector2D  operator-  ( Vector2D v )  { return Vector2D( x - v.x, y - v.y ); }
	Vector2D  operator*  ( float value ) { return Vector2D( x * value, y * value ); }
	Vector2D  operator/  ( float value ) { return Vector2D( x / value, y / value ); }
	Vector2D& operator+= ( Vector2D v )  { x += v.x; y += v.y; return *this; }
	Vector2D& operator-= ( Vector2D v )  { x -= v.x; y -= v.y; return *this; }
	Vector2D& operator*= ( float value ) { x *= value; y *= value; return *this; }
	Vector2D& operator/= ( float value ) { x /= value; y /= value; return *this; }
	Vector2D  operator-  ()              { return Vector2D ( -x, -y ); }

	float length ()	{ return sqrtf ( x*x + y*y );	}

	void unitize () {
		float length = this->length();
		if ( length == 0 )
			return;
		x /= length;
		y /= length;
	}

	Vector2D unit () {
		float length = this->length();
		if ( length == 0 )
			return *this;
		return Vector2D ( x / length, y / length );
	}

};

Vector2D::Vector2D() {
    x = 0;
    y = 0;
}

Vector2D::Vector2D( float _x, float _y ) {
    x = _x;
    y = _y;
}

Vector2D::~Vector2D() {

}

#endif // VECTOR3D_H
