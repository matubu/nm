#pragma once

#define NewResType(type) struct Res_ ## type { \
	char	*err; \
	type	data; \
}

#define Res(type) struct Res_ ## type
#define Returns(type) typedef Res(type) __returns_type

#define Ok(_data) (__returns_type){ .err = NULL, .data = (_data) }
#define Err(_err) (__returns_type){ .err = _err }
#define throw(_err) return (Err(_err))

#define catch(res, catch) \
	if (res.err) { catch }

#define unwrap(_res) ({ \
	typeof(_res)	res = (_res); \
	(res.err ? ({ throw(res.err); res.data; }) : res.data); \
})