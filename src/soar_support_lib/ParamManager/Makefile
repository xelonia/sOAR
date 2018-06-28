OBJ = LibconfigWrapper.o ParamManager.o

all: $(OBJ)
	$(LN) -o oar_support_lib.a $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -I../../../extern/include -c $<