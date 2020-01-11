it: mysudo.c
	gcc -o mysudo mysudo.c
	sudo chown root:root mysudo
	sudo chmod u+s mysudo
run:
	./mysudo

clean:
	rm mysudo
	