#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

_SUB STDTIME ST_Zero()
{

	STDTIME _ans = { 0, 0, 0, 0, 0, 0 };

	return(_ans);

}

_SUB DELTA_T ST_ZeroDelta()
{

	DELTA_T _ans = { 0, 0, 0, 0, 0, 0 };

	return(_ans);

}


_SUB STDTIME ST_End()
{
	STDTIME _ans = { 4000, 1, 0, 0, 0, 0 };

	return(_ans);
}
