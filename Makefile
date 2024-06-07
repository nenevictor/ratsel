app_name = ratsel

src = src/main.c \
      src/ratsel.c

vendor_src = vendor/hashmap/hashmap.c

debug:
	gcc -D _DEBUG -std=c99 -O0 -Wall -Wextra -Werror $(src) $(vendor_src) -o $(app_name)_d
	$(app_name)_d 

product:
	gcc -std=c99 -Ofast -Wall -Wextra -Werror $(src) $(vendor_src) -o $(app_name)