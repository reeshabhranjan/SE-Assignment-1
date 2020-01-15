it: mysudo.c
	gcc mysudo.c -o mysudo
	sudo chown root:root mysudo
	sudo chmod u+s mysudo
run:
	./mysudo

clean:
	rm mysudo
	
debug: mysudo.c
	gcc -g -o mysudo mysudo.c