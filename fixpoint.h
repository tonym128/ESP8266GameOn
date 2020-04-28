
#ifdef _WIN32
#include <stdint.h>
#elif __linux
#include <stdint.h>
typedef uint8_t byte;     // BYTE = unsigned 8 bit value
#elif ARDUINO
#include <Arduino.h>  // for type definitions
#elif __EMSCRIPTEN__
#include <stdint.h>
#endif

#define PI 3.14159265

// Fixed Point Math Helpers
#define FP32

#ifdef FP32
typedef int32_t FIXPOINT;
#define FP_SHIFT 16     // shifts to produce a fixed-point number
#define FP_SCALE 65536  // scaling factor
#define FIXP_FIXP_INT_PART(n) ((n) & 0xffff0000)
#define FIXP_DEC_PART(n) ((n) & 0x0000ffff)
#define FIXP_MULT(n,n2) ((FIXPOINT)((((int64_t)(n)) * n2) >> FP_SHIFT))
#define FIXP_DIV(n,n2) (FIXPOINT(((((int64_t)(n)) << FP_SHIFT) / (n2))))
#else
typedef int16_t FIXPOINT;
#define FP_SHIFT 8     // shifts to produce a fixed-point number
#define FP_SCALE 256  // scaling factor
#define FIXP_FIXP_INT_PART(n) ((n) & 0xff00)
#define FIXP_DEC_PART(n) ((n) & 0x00ff)
#define FIXP_MULT(n,n2) ((FIXPOINT)((((int32_t)(n)) * n2) >> FP_SHIFT))
#define FIXP_DIV(n,n2) (FIXPOINT(((((int32_t)(n)) << FP_SHIFT) / (n2))))
#endif

#define INT_TO_FIXP(n) (FIXPOINT(((n) << FP_SHIFT)))
#define FIXP_TO_INT(n) (int(((n) >> FP_SHIFT)))
#define FLOAT_TO_FIXP(n) (FIXPOINT((float)(n) * FP_SCALE))
#define FIXP_TO_FLOAT(n) (float((float)(n) / FP_SCALE))
#define FIXP_INT_PART(n) ((n) >> FP_SHIFT)
#define FIXEDPT_PI FLOAT_TO_FIXP(PI)
#define FIXEDPT_HALF_PI FLOAT_TO_FIXP(PI/2)
#define FIXP_1 INT_TO_FIXP(1)

/* Returns the sine of the given fixedpt number. 
 * Note: the loss of precision is extraordinary! */
const FIXPOINT SK[2] = {
	FLOAT_TO_FIXP(7.61e-03),
	FLOAT_TO_FIXP(1.6605e-01)
};

static FIXPOINT
FIXPOINT_SIN(FIXPOINT fp)
{
	int sign = 1;
	FIXPOINT sqr, result;

	fp %= 2 * FIXEDPT_PI;
	if (fp < 0)
		fp = FIXEDPT_PI * 2 + fp;
	if ((fp > FIXEDPT_HALF_PI) && (fp <= FIXEDPT_PI)) 
		fp = FIXEDPT_PI - fp;
	else if ((fp > FIXEDPT_PI) && (fp <= (FIXEDPT_PI + FIXEDPT_HALF_PI))) {
		fp = fp - FIXEDPT_PI;
		sign = -1;
	} else if (fp > (FIXEDPT_PI + FIXEDPT_HALF_PI)) {
		fp = (FIXEDPT_PI << 1) - fp;
		sign = -1;
	}
	sqr = FIXP_MULT(fp, fp);
	result = SK[0];
	result = FIXP_MULT(result, sqr);
	result -= SK[1];
	result = FIXP_MULT(result, sqr);
	result += FIXP_1;
	result = FIXP_MULT(result, fp);
	return sign * result;
}

/* Returns the cosine of the given fixedpt number */
static FIXPOINT
FIXPOINT_COS(FIXPOINT A)
{
	return (FIXPOINT_SIN(FIXEDPT_HALF_PI - A));
}


/* Returns the tangens of the given fixedpt number */
static FIXPOINT
FIXPOINT_TAN(FIXPOINT A)
{
	return FIXP_DIV(FIXPOINT_SIN(A), FIXPOINT_COS(A));
}

static FIXPOINT sqrtF2F ( FIXPOINT x )
{
    uint32_t t, q, b, r;
    r = x;
    b = 0x40000000;
    q = 0;
    while( b > 0x40 )
    {
        t = q + b;
        if( r >= t )
        {
            r -= t;
            q = t + b; // equivalent to q += 2*b
        }
        r <<= 1;
        b >>= 1;
    }
    q >>= 8;
    return q;
}

static FIXPOINT xVec(FIXPOINT speed, FIXPOINT direction)
{

	if (direction == INT_TO_FIXP(90))
		return speed;
	if (direction == INT_TO_FIXP(270))
		return -speed;

	if (direction == INT_TO_FIXP(0) || direction == INT_TO_FIXP(180))
		return 0;
	int sign = 1;

	if (direction < INT_TO_FIXP(90))
	{
	}
	else if (direction < INT_TO_FIXP(180))
	{
		direction = direction - INT_TO_FIXP(90);
		direction = INT_TO_FIXP(90) - direction;
	}
	else if (direction < INT_TO_FIXP(270))
	{
		// X Value is -'ve
		sign = -1;
		direction = direction - INT_TO_FIXP(180);
	}
	else
	{
		// X Value is -'ve
		direction = direction - INT_TO_FIXP(270);
		direction = INT_TO_FIXP(90) - direction;
		sign = -1;
	}

	return FIXP_MULT(speed * sign, FIXP_DIV(direction, INT_TO_FIXP(90)));
}

static FIXPOINT yVec(FIXPOINT speed, FIXPOINT direction)
{
	if (direction == INT_TO_FIXP(90) || direction == INT_TO_FIXP(270))
		return 0;

	if (direction == INT_TO_FIXP(0))
		return -speed;
	if (direction == INT_TO_FIXP(180))
		return speed;
	int sign = 1;

	if (direction < INT_TO_FIXP(90))
	{
		direction = INT_TO_FIXP(90) - direction;
		sign = -1;
	}
	else if (direction < INT_TO_FIXP(180))
	{
		direction = direction - INT_TO_FIXP(90);
	}
	else if (direction < INT_TO_FIXP(270))
	{
		direction = direction - INT_TO_FIXP(180);
		direction = INT_TO_FIXP(90) - direction;
	}
	else
	{
		direction = direction - INT_TO_FIXP(270);
		sign = -1;
	}

	return FIXP_MULT(speed * sign, FIXP_DIV(direction, INT_TO_FIXP(90)));
}

