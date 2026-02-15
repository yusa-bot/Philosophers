

// ft_atoi
static int	check_overflow(int sign, long current, char next)
{
	int	digit;

	digit = next - '0';
	if (sign == 1)
	{
		if (current > LONG_MAX / 10
			|| (current == LONG_MAX / 10 && digit > LONG_MAX % 10))
			return (1);
	}
	if (sign == -1)
	{
		if (current > -(LONG_MIN / 10)
			|| (current == -(LONG_MIN / 10) && digit > -(LONG_MIN % 10)))
			return (-1);
	}
	return (0);
}
int	ft_atoi(const char *str)
{
	int					sign;
	unsigned long int	result;

	sign = 1;
	result = 0;
	while (*str == 32 || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-')
	{
		sign = -1;
		str++;
	}
	else if (*str == '+')
		str++;
	while (*str >= '0' && *str <= '9')
	{
		if (check_overflow(sign, result, *str) == 1)
			return ((int)LONG_MAX);
		if (check_overflow(sign, result, *str) == -1)
			return ((int)LONG_MIN);
		result = result * 10 + (long)(*str - '0');
		str++;
	}
	return ((int)(result * sign));
}