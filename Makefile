app_name = ratsel

vendor_src = vendor/hashmap/hashmap.c
src = src/main.c \
	  src/ratsel.c

libs = -lraylib -lopengl32 -lgdi32 -lwinmm
include = vendor

debug:
	gcc -D _DEBUG -std=c99 -O0 -Wall -Wextra -Werror -I$(include) $(src) $(vendor_src) -o $(app_name)_d
	$(app_name)_d

product:
	gcc -std=c99 -Ofast -Wall -Wextra -Werror -I$(include) $(src) $(vendor_src) -o $(app_name)
	$(app_name)