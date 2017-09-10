PATH      = C:\windows;c:\windows\system32;c:\gcc\build\bin

NAME      = Cookshop-MIL300

GCCDIR    = C:\gcc\build
GCCBIN    = $(GCCDIR)\bin
GCCLIB    = $(GCCDIR)\lib
GCCLIBELF = $(GCCDIR)\libelf
GCCINC    = $(GCCDIR)\include 

LOCSRC    = .\src          					
LOCOBJ    = .\obj               				
LOCINC    = .\inc      
LOCLIB    = .\lib

LIB       = $(GCCLIB)\libc.a
JOSLIBDIR = .\lib
JOSLIB    = pc1000api(v20) 
WLSLIB    = pc1000wlsapi(v20)

ASM       = $(GCCDIR)\BIN\arm-elf-as  
GCC       = $(GCCDIR)\BIN\arm-elf-gcc -O3 -I$(GCCINC) -I$(LOCINC) -mlittle-endian -mcpu=arm9 -c
LINK      = $(GCCDIR)\BIN\arm-elf-ld -Tldscript -L$(GCCLIB) -L$(GCCLIBELF) -L$(JOSLIBDIR)
CCFLAG	  = -O3 -I.\build\include -mlittle-endian -mcpu=arm9

OBJ       = $(LOCOBJ)\init.o $(LOCOBJ)\main.o $(LOCOBJ)\public.o $(LOCOBJ)\sms_pdu.o $(LOCOBJ)\bencode.o $(LOCOBJ)\cookshop.o
		 
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

$(LOCOBJ)\public.o: $(LOCSRC)\public.c
        $(GCC) $(LOCSRC)\public.c -o $(LOCOBJ)\public.o

$(LOCOBJ)\sms_pdu.o: $(LOCSRC)\sms_pdu.c
        $(GCC) $(LOCSRC)\sms_pdu.c -o $(LOCOBJ)\sms_pdu.o

$(LOCOBJ)\bencode.o: $(LOCSRC)\bencode.c
        $(GCC) $(LOCSRC)\bencode.c -o $(LOCOBJ)\bencode.o

$(LOCOBJ)\cookshop.o: $(LOCSRC)\cookshop.c
        $(GCC) $(LOCSRC)\cookshop.c -o $(LOCOBJ)\cookshop.o

clean:
        del $(LOCOBJ)\*.o
        del *.bin
        del *.elf
