run:
	cpp some.s | yasm - -o some
	xxd -ps some | tr -d '\n' | sed 's/\(..\)/0x\1, /g'

# ld some.o
# ./a.out
