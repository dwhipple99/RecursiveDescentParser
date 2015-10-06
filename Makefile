OBJS = dwbasic.o dwbasic-interface.o log-module.o

dwbasic			: ${OBJS}
			CC -o dwbasic ${OBJS}

log-module.o		: log-module.c dwbasic.h
			CC -c log-module.c

dwbasic-interface.o	: dwbasic-interface.c dwbasic.h
			CC -c dwbasic-interface.c

dwbasic.o		: dwbasic.c dwbasic.h
			CC -c dwbasic.c

clean			:
			rm ${OBJS} dwbasic
