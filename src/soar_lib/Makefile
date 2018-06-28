OBJ = Backward.o Decision.o Forward.o Individual.o Optimizer.o Settings.o StateFuncs.o

all: $(OBJ)
	$(LN) -o oar_support_lib.a $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $<