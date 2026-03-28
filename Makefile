
live:
	sudo make -C bee.fish/code PORT=443

clean:
	sudo make -C bee.fish/code clean
	
debug:
	make -C dev.bee.fish/code debug PORT=8000

clean_debug:
	make -C dev.bee.fish/code clean
