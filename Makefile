# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/17 20:20:42 by ayusa             #+#    #+#              #
#    Updated: 2026/02/18 14:50:13 by ayusa            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = philo

SRCS = main.c init.c philo.c monitor.c cleanup.c util.c
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

CC = cc
CFLAGS = -Wall -Wextra -Werror -MMD -MP
INCS = -I.
RM = rm -f

%.o: %.c philo.h
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) -lpthread

clean:
	$(RM) $(OBJS) $(DEPS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re

void	log_print(t_philo *philo, char *msg)
{
	long long	now;
	long long	timestamp;
	int			now_flag;

	now = get_time_us();
	timestamp = (now - philo->data->start_time) / 1000;

	pthread_mutex_lock(&philo->data->stop_flag_mutex);
	now_flag = philo->data->stop_flag;
	pthread_mutex_unlock(&philo->data->stop_flag_mutex);

	if (!now_flag)
	{
		pthread_mutex_lock(&philo->data->log_mutex);
		printf("%lld %zu %s\n", timestamp, philo->x, msg);
		pthread_mutex_unlock(&philo->data->log_mutex);
	}
}