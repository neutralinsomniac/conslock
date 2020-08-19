</$objtype/mkfile

conslock:   conslock.$O
	$LD $LDFLAGS -o $target $prereq

%.$O:   %.c
	$CC $CFLAGS $stem.c
