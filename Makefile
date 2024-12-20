APP = $(APP_NAME)

APP_NAME=ft_ping #create variable called APP_NAME and assign value to it which is ft_ping

# sources
SRCS_DIR = src
HEADERS_DIR = headers
SRCS = $(addprefix $(SRCS_DIR)/, main.c sigint_handler.c ping_session.c resources.c ping.c net.c debug.c ping_session_stat.c parse_args.c)

HEADERS = $(addprefix $(HEADERS_DIR)/, ping_session.h sigint_handler.h exit_constants.h resources.h ping.h net.h debug.h ping_session_stat.h parse_args.h)

# -I followed by dir name - it is dirrectory where compiler will find *.h files
# during compilation process

# objs

OBJS = $(SRCS:.c=.o)

# flags

CC = gcc # Flag for implicit rules

# gcc - is compiler gcc hello.c is to compile file
# -Wall -Wextra -Werror
CFLAGS =  -c -g -Wall -Wextra -Werror # Flag for implicit rules.
# -Wall - show all error messager
# -c compile (produce *.o files but not link)
# -g set symbolic points in file, so debugging is possible

# -L followed by path is where libraries can be found
# -l followed by name of library is we specify name of library compiler needs
# library name in Unix is libxxxxx.a 
# these two flags are needed by linker

.PHONY: all clean fclean re run #these targets will not be treated as targets that produce files. Target that produce file gets executed if file doesn't exist or if file exists but it its dependecies are fresher. Phone targets gets executed every time they are called
#below is rule. all - rule name, after ":" go dependency that is needed to execute rule
#all is the rule common to all make files. By convention it must compile all system
all: $(APP)

$(APP): $(OBJS) #here this OBJS variable is expaned to list of .o files. And make will go and look for rule that corresponds to name.o file. And it will find this rule in %.o : %.c
	@ echo target $(APP_NAME)
	@ $(CC) -o $(APP) $(OBJS) -lm #-lm to link math library, otherwise can not compile
# -o is compiler flag to produce object *.o file with particular name 

#below is % is a vild card. We say that if we want to produce %.o (whatever name dot o) then we need as dependency same name dot c
#$@ this is local variable $@ means take whatever it is on left side of colon in %.o : %.c
# @ echo target means that command itself will not be printed to console
#$(HEADERS) <- ADD WHEN I HAVE HEADERS
# $< - this referes to first prerequisite. i.e. in %.o: %.c it is %.c

%.o: %.c $(HEADERS)
	@ echo target $@
	$(CC) $(CFLAGS) -I$(HEADERS_DIR) $< -o $@

clean:
	rm -f $(OBJS)

fclean:
	@ echo remove program file
	@ rm -f $(APP_NAME)

re: clean fclean all

run: re
	./ft_ping $(HOST) $(DEBUG)

# %.o : %.c
# %.o - target that corresponds to every .o file.
# %.c - converts .o target to .c file name aa.o -> aa.c so we get aa.c prerequisit

#  $@ automatic variable that contains the target name.
#  $< automatic variable that contains the prerequisit name.

