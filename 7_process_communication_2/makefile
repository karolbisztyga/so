test: test.c
	gcc -Wall -ansi -pedantic test.c -o p.exe

fifo: cw1/srvfifo.c cw1/cntfifo.c
	gcc cw1/srvfifo.c -o srvfifo.exe ; gcc cw1/cntfifo.c -o cntfifo.exe

sv: cw2/srvsv.c cw2/cntsv.c
	gcc cw2/srvsv.c -o srvsv.exe ; gcc cw2/cntsv.c -o cntsv.exe

psx: cw3/srvpsx.c cw3/cntpsx.c
	gcc -lrt cw3/srvpsx.c -o srvpsx.exe ; gcc -lrt cw3/cntpsx.c -o cntpsx.exe

clean:
	rm *.exe