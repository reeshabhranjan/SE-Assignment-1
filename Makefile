it: mysudo.c
	gcc mysudo.c -o mysudo -lcrypt
	sudo chown root:root mysudo
	sudo chmod u+s mysudo
run:
	./mysudo

clean:
	rm mysudo
	
debug: modeuid.c diseuid.c
	gcc -o modeuid modeuid.c
	gcc -o diseuid diseuid.c
	sudo chown root:root modeuid
	sudo chmod u+s modeuid