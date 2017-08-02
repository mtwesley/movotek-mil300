PATH      = C:\windows;c:\windows\system32;c:\gcc\build\bin

NAME      = Cookshop-MIL300

GCCDIR    = C:\gcc\build
GCCBIN    = $(GCCDIR)\bin
GCCLIB    = $(GCCDIR)\lib
GCCLIBELF = $(GCCDIR)\libelf
GCCINC    = $(GCCDIR)\include 

LOCSRC    = .\src          					
LOCOBJ    = .\obj               				
LOCINC    = $(GCCINC)      
LOCLIB    = ..\lib

LIB       = $(GCCLIB)\libc.a
JOSLIBDIR = ..\lib
JOSLIB    = pc1000api(v20) 
WLSLIB    = pc1000wlsapi(v20)

ASM       = $(GCCDIR)\BIN\arm-elf-as  
GCC       = $(GCCDIR)\BIN\arm-elf-gcc -O3 -I$(GCCINC) -I..\inc  -mlittle-endian -mcpu=arm9 -c
LINK      = $(GCCDIR)\BIN\arm-elf-ld -Tldscript -L$(GCCLIB) -L$(GCCLIBELF) -L$(JOSLIBDIR)
CCFLAG	  = -O3 -I..\build\include -mlittle-endian -mcpu=arm9

OBJ    = $(LOCOBJ)\init.o \
         $(LOCOBJ)\main.o \
		 $(LOCOBJ)\kbtest.o \
		 $(LOCOBJ)\public.o \
		 $(LOCOBJ)\systest.o \
		 $(LOCOBJ)\comtest.o \
		 $(LOCOBJ)\wlstest.o \
		 $(LOCOBJ)\prnapi.o \
		 $(LOCOBJ)\picc.o 
		 
#  ADS-ROM version
$(NAME).elf: $(OBJ)
    $(LINK) -o $(NAME).elf $(OBJ)  -l$(WLSLIB) -l$(JOSLIB)  -lc  -lgcc
    elftobin $(NAME).elf $(NAME).bin PC1000---APP

# ASM file
$(LOCOBJ)\init.o: $(LOCSRC)\init.s
        $(ASM) $(LOCSRC)\init.s -o $(LOCOBJ)\init.o
	
#  C files
$(LOCOBJ)\main.o: $(LOCSRC)\main.c
        $(GCC) $(LOCSRC)\main.c -o $(LOCOBJ)\main.o

$(LOCOBJ)\kbtest.o: $(LOCSRC)\kbtest.c
        $(GCC) $(LOCSRC)\kbtest.c -o $(LOCOBJ)\kbtest.o
 
$(LOCOBJ)\public.o: $(LOCSRC)\public.c
        $(GCC) $(LOCSRC)\public.c -o $(LOCOBJ)\public.o

$(LOCOBJ)\systest.o: $(LOCSRC)\systest.c
        $(GCC) $(LOCSRC)\systest.c -o $(LOCOBJ)\systest.o
  
$(LOCOBJ)\comtest.o: $(LOCSRC)\comtest.c
        $(GCC) $(LOCSRC)\comtest.c -o $(LOCOBJ)\comtest.o
        
$(LOCOBJ)\prnapi.o: $(LOCSRC)\prnapi.c
        $(GCC) $(LOCSRC)\prnapi.c -o $(LOCOBJ)\prnapi.o 
 
$(LOCOBJ)\wlstest.o: $(LOCSRC)\wlstest.c
        $(GCC) $(LOCSRC)\wlstest.c -o $(LOCOBJ)\wlstest.o 

$(LOCOBJ)\picc.o: $(LOCSRC)\picc.c
        $(GCC) $(LOCSRC)\picc.c -o $(LOCOBJ)\picc.o 

clean:
	del *.db
	del *.blk
	del *.db2
	del *.out
