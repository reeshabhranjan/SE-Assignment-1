build: mysudo.c
	gcc mysudo.c -o mysudo
	sudo chown root:root mysudo
	sudo chmod 755 mysudo
	sudo chmod u+s mysudo
	
debug: mysudo.c
	gcc -g -o mysudo mysudo.c
	gdb mysudo