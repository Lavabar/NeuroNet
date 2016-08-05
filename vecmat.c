#include <math.h>
#include <string.h>

struct vector2 {
	double x, y;
};

struct matrix3
{
	double _11, _12, _13,
		_21, _22, _23,
		_31, _32, _33;
};

double v3dist(struct vector2 *v0, struct vector2 *v1)
{
	double xdist;
	double ydist;
	
	xdist = v1->x - v0->x;
	ydist = v1->y - v0->y;

	return sqrt(pow(xdist, 2.0) + pow(ydist, 2.0));
}

int m3scale(struct matrix3 *r, double x, double y)
{
	r->_11 = x;	r->_12 = 0.0;	r->_13 = 0.0;
	r->_21 = 0.0;	r->_22 = y;	r->_23 = 0.0;
	r->_31 = 0.0;	r->_32 = 0.0;	r->_33 = 1.0;

	return 0;
}

int m3rotate(struct matrix3 *r, double ang)
{
	r->_11 = cos(ang);	r->_12 = sin(ang);	r->_13 = 0.0;
	r->_21 = -sin(ang);	r->_22 = cos(ang);	r->_23 = 0.0;
	r->_31 = 0.0;		r->_32 = 0.0;		r->_33 = 1.0;

	return 0;
}

int m3translate(struct matrix3 *r, double x, double y)
{
	r->_11 = 1.0;	r->_12 = 0.0;	r->_13 = 0.0;
	r->_21 = 0.0;	r->_22 = 1.0;	r->_23 = 0.0;
	r->_31 = x;	r->_32 = y;	r->_33 = 1.0;

	return 0;
}

int v2m3mult(struct vector2 *v, struct matrix3 *m, struct vector2 *r)
{
	struct vector2 tmp;

	tmp.x = m->_11 * v->x + m->_21 * v->y + m->_31;
	tmp.y = m->_12 * v->x + m->_22 * v->y + m->_32;

	memcpy(r, &tmp, sizeof(struct vector2));

	return 0;
}

int m3mult(struct matrix3 *m0, struct matrix3 *m1, struct matrix3 *r)
{
	struct matrix3 tmp;

	tmp._11 = (m1->_11 * m0->_11) + (m1->_21 * m0->_12) + (m1->_31 * m0->_13);
	tmp._12 = (m1->_12 * m0->_11) + (m1->_22 * m0->_12) + (m1->_32 * m0->_13);
	tmp._13 = (m1->_13 * m0->_11) + (m1->_23 * m0->_12) + (m1->_33 * m0->_13);

	tmp._21 = (m1->_11 * m0->_21) + (m1->_21 * m0->_22) + (m1->_31 * m0->_23);
	tmp._22 = (m1->_12 * m0->_21) + (m1->_22 * m0->_22) + (m1->_32 * m0->_23);
	tmp._23 = (m1->_13 * m0->_21) + (m1->_23 * m0->_22) + (m1->_33 * m0->_23);

	tmp._31 = (m1->_11 * m0->_31) + (m1->_21 * m0->_32) + (m1->_31 * m0->_33);
	tmp._32 = (m1->_12 * m0->_31) + (m1->_22 * m0->_32) + (m1->_32 * m0->_33);
	tmp._33 = (m1->_13 * m0->_31) + (m1->_23 * m0->_32) + (m1->_33 * m0->_33);

	memcpy(r, &tmp, sizeof(struct matrix3));

	return 0;
}

int m3inverse(struct matrix3 *m, struct matrix3 *r)
{
	double invM[3][3], tmpM[3][3];
	double coef;
	int i, j, k;

	memcpy(tmpM, m, sizeof(struct matrix3));

	for (i = 0; i < 3; ++i)
		for (j = 0; j < 3; ++j)
			invM[i][j] = (i == j) ? 1.0f : 0.0;

	for (k = 0; k < 3; ++k) {
		coef = 1/tmpM[k][k];

		for ( j = 0; j < 3; ++j ) {
			tmpM[k][j] *= coef;
			invM[k][j] *= coef;
		}

		for (i = 0; i < 3; ++i)
			if (i != k) {
				coef = -(tmpM[i][k] / tmpM[k][k]);
				
				for (j = 0; j < 3; ++j) {
					tmpM[i][j] += tmpM[k][j]*coef;
					invM[i][j] += invM[k][j]*coef;
				}
			}
	}

	memcpy(r, invM, sizeof(struct matrix3));
	
	return 0;
}
