test: test.c
	gcc -Wall -ansi -pedantic test.c -o p.exe

fifo: cw1/srvfifo.c cw1/cntfifo.c
	gcc cw1/srvfifo.c -o srvfifo.exe ; gcc cw1/cntfifo.c -o cntfifo.exe

sv: cw2/srvsv.c cw2/cntsv.c
	gcc cw2/srvsv.c -o srvsv.exe ; gcc cw2/cntsv.c -o cntsv.exe

psx: cw3/srvpsx.c cw3/cntpsx.c
	gcc -lrt cw3/srvpsx.c -o srvpsx.exe -lrt ; gcc -lrt cw3/cntpsx.c -o cntpsx.exe -lrt

eliza: chatbot/eliza.c
	gcc -Wall -ansi -pedantic chatbot/eliza.c -o eliza.exe

chatbot: chatbot/server.c chatbot/client.c
	gcc -Wall -ansi -pedantic -std=gnu99 chatbot/server.c -o server_chatbot.exe;
	gcc -Wall -ansi -pedantic -std=gnu99 chatbot/client.c -o client_chatbot.exe

votes_counter: votes/client.c votes/server.c
	gcc -Wall -ansi -pedantic -std=gnu99 votes/client.c -o client_votes.exe -lrt;
	gcc -Wall -ansi -pedantic -std=gnu99 votes/server.c -o server_votes.exe -lrt

clean:
	rm *.exe