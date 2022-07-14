static inline int	static_inline_add(int a, int b)
{
	return (a + b);
}

int	main()
{
	int ret = static_inline_add(1, 7);

	return (ret);
}