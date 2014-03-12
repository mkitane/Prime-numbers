COMP = gcc
EDL = gcc
RM = rm 
EXE = tpThread
CLEAN = efface
EXEFLAGS = -Wno-format
CPPFLAGS = 
RMFLAGS = -f


.PHONY : $(CLEAN)

$(EXE) : src/main.c
	$(EDL) $(EXEFLAGS) -o $(EXE) $< -lm

$(CLEAN) :
	$(RM) $(RMFLAGS) *.o $(EXE) core
