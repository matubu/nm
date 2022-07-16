#include <stdio.h>


#include "./incs/res.h"


NewResType(int);
NewResType(float);

Res(int) test(int a, int b)
{
	Returns(int);
	// throw("add error");

	return Ok(a + b);
}

Res(int) test_wrap(int a, int b)
{
	Returns(int);

	int result = unwrap(test(a, b));

	printf("-> %d\n", result);
	return Ok(result);
}

Res(float) test_float(int a, int b)
{
	Returns(float);

	return Ok(.5);
}

int	main() {
	Res(int) result = test_wrap(1, 2);

	catch(result, {
		printf("fail: %s\n", result.err);
		return (-1);
	});
	printf("success: %d\n", result.data);

	return (0);
}