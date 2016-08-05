#ifndef VECMAT_H
#define VECMAT_H

struct vector2 {
	double x, y;
};

struct matrix3
{
	double _11, _12, _13,
		_21, _22, _23,
		_31, _32, _33;
};

double v3dist(struct vector2 *v0, struct vector2 *v1);

int m3scale(struct matrix3 *r, double x, double y);

int m3rotate(struct matrix3 *r, double ang);

int m3translate(struct matrix3 *r, double x, double y);

int v2m3mult(struct vector2 *v, struct matrix3 *m, struct vector2 *r);

int m3mult(struct matrix3 *m0, struct matrix3 *m1, struct matrix3 *r);

int m3inverse(struct matrix3 *m, struct matrix3 *r);

#endif
