# Generated automatically from Makefile.in by configure.


SHELL = /bin/sh

all: input output

input: input.txt input.dvi input.ps

input.txt: input-ascii.txt
	cp input-ascii.txt input.txt

input.dvi: input-dvi.tex
	cp input-dvi.tex input.tex
	latex input.tex
	rm input.log input.aux input.tex

input.ps: input.dvi
	dvips input.dvi

clean:
	rm -f input.txt input.dvi input.ps


output:
	@./testnlp 01-txt      ''                 input.txt
	@./testnlp 02-txt-B    '-B'               input.txt
	@./testnlp 03-txt-F4   '-F4'              input.txt
	@./testnlp 04-txt-w    '-w'               input.txt
	@./testnlp 05-txt-l40  '-l40'             input.txt
	@./testnlp 06-txt-N    '-N'               input.txt
	@./testnlp 07-txt-Hs   '-H "Test Header"' input.txt
	@./testnlp 08-txt-m    '-m'               input.txt
	@./testnlp 09-txt-L    '-L'               input.txt
	@./testnlp 10-txt-b    '-b'               input.txt
	@./testnlp 11-txt-2    '-2'               input.txt
	@./testnlp 12-txt-2b   '-2 -b'            input.txt
	@./testnlp 13-txt-L2   '-2 -L'            input.txt
	@./testnlp 14-txt-L2b  '-2 -L -b'         input.txt
	@./testnlp 15-txt-4    '-4'               input.txt
	@./testnlp 16-txt-4b   '-4 -b'            input.txt
	@./testnlp 17-txt-L4   '-4 -L'            input.txt
	@./testnlp 18-txt-L4b  '-4 -L -b'         input.txt
	@./testnlp 19-dvi      ''                 input.dvi
	@./testnlp 20-dvi-R600 '-R600'            input.dvi
	@./testnlp 21-ps       ''                 input.ps
	@./testnlp 22-ps-L     '-L'               input.ps
	@./testnlp 23-ps-b     '-b'               input.ps
	@./testnlp 24-ps-2     '-2'               input.ps
	@./testnlp 25-ps-2b    '-2 -b'            input.ps
	@./testnlp 26-ps-L2    '-2 -L'            input.ps
	@./testnlp 27-ps-L2b   '-2 -L -b'         input.ps
	@./testnlp 28-ps-4     '-4'               input.ps
	@./testnlp 29-ps-4b    '-4 -b'            input.ps
	@./testnlp 30-ps-L4    '-4 -L'            input.ps
	@./testnlp 31-ps-L4b   '-4 -L -b'         input.ps

