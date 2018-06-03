a.out:
	gcc ex2.c

check:
	echo -e '\E[37;44m'"\033[1m/////// START//////\033[0m"
	echo -e '\E[37;44m'"\033[1m/////// Testing mission 1//////\033[0m"

	./a.out mac_input-utf-16.txt dst.txt
	cmp dst.txt mac_input-utf-16.txt

	./a.out unix_input-utf-16.txt dst.txt
	cmp dst.txt unix_input-utf-16.txt

	./a.out win_input-utf-16.txt dst.txt
	cmp dst.txt win_input-utf-16.txt

	echo -e '\E[37;44m'"\033[1m/////// Testing mission 2//////\033[0m"

	./a.out mac_input-utf-16.txt dst.txt -mac -unix
	cmp dst.txt unix_input-utf-16.txt

	./a.out mac_input-utf-16.txt dst.txt -mac -win
	cmp dst.txt win_input-utf-16.txt

	./a.out mac_input-utf-16.txt dst.txt -mac -mac
	cmp dst.txt mac_input-utf-16.txt

	./a.out unix_input-utf-16.txt dst.txt -unix -mac
	cmp dst.txt mac_input-utf-16.txt

	./a.out unix_input-utf-16.txt dst.txt -unix -win
	cmp dst.txt win_input-utf-16.txt

	./a.out unix_input-utf-16.txt dst.txt -unix -unix
	cmp dst.txt unix_input-utf-16.txt

	./a.out win_input-utf-16.txt dst.txt -win -mac
	cmp dst.txt mac_input-utf-16.txt

	./a.out win_input-utf-16.txt dst.txt -win -unix
	cmp dst.txt unix_input-utf-16.txt

	./a.out win_input-utf-16.txt dst.txt -win -win
	cmp dst.txt win_input-utf-16.txt

	echo -e '\E[37;44m'"\033[1m/////// Testing mission 3 keep//////\033[0m"

	./a.out mac_input-utf-16.txt dst.txt -mac -unix -keep
	cmp dst.txt unix_input-utf-16_keep.txt

	./a.out mac_input-utf-16.txt dst.txt -mac -win -keep
	cmp dst.txt win_input-utf-16_keep.txt

	./a.out mac_input-utf-16.txt dst.txt -mac -mac -keep
	cmp dst.txt mac_input-utf-16_keep.txt

	./a.out unix_input-utf-16.txt dst.txt -unix -mac -keep
	cmp dst.txt mac_input-utf-16_keep.txt

	./a.out unix_input-utf-16.txt dst.txt -unix -win -keep
	cmp dst.txt win_input-utf-16_keep.txt

	./a.out unix_input-utf-16.txt dst.txt -unix -unix -keep
	cmp dst.txt unix_input-utf-16_keep.txt

	./a.out win_input-utf-16.txt dst.txt -win -mac -keep
	cmp dst.txt mac_input-utf-16_keep.txt

	./a.out win_input-utf-16.txt dst.txt -win -unix -keep
	cmp dst.txt unix_input-utf-16_keep.txt

	./a.out win_input-utf-16.txt dst.txt -win -win -keep
	cmp dst.txt win_input-utf-16_keep.txt

	echo "/////// Testing mission 3 swap///////"

	./a.out mac_input-utf-16.txt dst.txt -mac -unix -swap
	cmp dst.txt unix_input-utf-16_swap.txt

	./a.out mac_input-utf-16.txt dst.txt -mac -win -swap
	cmp dst.txt win_input-utf-16_swap.txt

	./a.out mac_input-utf-16.txt dst.txt -mac -mac -swap
	cmp dst.txt mac_input-utf-16_swap.txt

	./a.out unix_input-utf-16.txt dst.txt -unix -mac -swap
	cmp dst.txt mac_input-utf-16_swap.txt

	./a.out unix_input-utf-16.txt dst.txt -unix -win -swap
	cmp dst.txt win_input-utf-16_swap.txt

	./a.out unix_input-utf-16.txt dst.txt -unix -unix -swap
	cmp dst.txt unix_input-utf-16_swap.txt

	./a.out win_input-utf-16.txt dst.txt -win -mac -swap
	cmp dst.txt mac_input-utf-16_swap.txt

	./a.out win_input-utf-16.txt dst.txt -win -unix -swap
	cmp dst.txt unix_input-utf-16_swap.txt

	./a.out win_input-utf-16.txt dst.txt -win -win -swap
	cmp dst.txt win_input-utf-16_swap.txt

	echo -e '\E[37;44m'"\033[1m/////// Testing mission 1 swap files//////\033[0m"

	./a.out mac_input-utf-16_swap.txt dst.txt
	cmp dst.txt mac_input-utf-16_swap.txt

	./a.out unix_input-utf-16_swap.txt dst.txt
	cmp dst.txt unix_input-utf-16_swap.txt

	./a.out win_input-utf-16_swap.txt dst.txt
	cmp dst.txt win_input-utf-16_swap.txt

	echo -e '\E[37;44m'"\033[1m/////// Testing mission 2 with swap files//////\033[0m"

	./a.out mac_input-utf-16_swap.txt dst.txt -mac -unix
	cmp dst.txt unix_input-utf-16_swap.txt

	./a.out mac_input-utf-16_swap.txt dst.txt -mac -win
	cmp dst.txt win_input-utf-16_swap.txt

	./a.out mac_input-utf-16_swap.txt dst.txt -mac -mac
	cmp dst.txt mac_input-utf-16_swap.txt

	./a.out unix_input-utf-16_swap.txt dst.txt -unix -mac
	cmp dst.txt mac_input-utf-16_swap.txt

	./a.out unix_input-utf-16_swap.txt dst.txt -unix -win
	cmp dst.txt win_input-utf-16_swap.txt

	./a.out unix_input-utf-16_swap.txt dst.txt -unix -unix
	cmp dst.txt unix_input-utf-16_swap.txt

	./a.out win_input-utf-16_swap.txt dst.txt -win -mac
	cmp dst.txt mac_input-utf-16_swap.txt

	./a.out win_input-utf-16_swap.txt dst.txt -win -unix
	cmp dst.txt unix_input-utf-16_swap.txt

	./a.out win_input-utf-16_swap.txt dst.txt -win -win
	cmp dst.txt win_input-utf-16_swap.txt

	echo -e '\E[37;44m'"\033[1m/////// Testing mission 3 with swap files - keep//////\033[0m"

	./a.out mac_input-utf-16_swap.txt dst.txt -mac -unix -keep
	cmp dst.txt unix_input-utf-16_swap.txt

	./a.out mac_input-utf-16_swap.txt dst.txt -mac -win -keep
	cmp dst.txt win_input-utf-16_swap.txt

	./a.out mac_input-utf-16_swap.txt dst.txt -mac -mac -keep
	cmp dst.txt mac_input-utf-16_swap.txt

	./a.out unix_input-utf-16_swap.txt dst.txt -unix -mac -keep
	cmp dst.txt mac_input-utf-16_swap.txt

	./a.out unix_input-utf-16_swap.txt dst.txt -unix -win -keep
	cmp dst.txt win_input-utf-16_swap.txt

	./a.out unix_input-utf-16_swap.txt dst.txt -unix -unix -keep
	cmp dst.txt unix_input-utf-16_swap.txt

	./a.out win_input-utf-16_swap.txt dst.txt -win -mac -keep
	cmp dst.txt mac_input-utf-16_swap.txt

	./a.out win_input-utf-16_swap.txt dst.txt -win -unix -keep
	cmp dst.txt unix_input-utf-16_swap.txt

	./a.out win_input-utf-16_swap.txt dst.txt -win -win -keep
	cmp dst.txt win_input-utf-16_swap.txt

	echo -e '\E[37;44m'"\033[1m/////// Testing mission 3 with swap files - swap//////\033[0m"

	./a.out mac_input-utf-16_swap.txt dst.txt -mac -unix -swap
	cmp dst.txt unix_input-utf-16.txt

	./a.out mac_input-utf-16_swap.txt dst.txt -mac -win -swap
	cmp dst.txt win_input-utf-16.txt

	./a.out mac_input-utf-16_swap.txt dst.txt -mac -mac -swap
	cmp dst.txt mac_input-utf-16.txt

	./a.out unix_input-utf-16_swap.txt dst.txt -unix -mac -swap
	cmp dst.txt mac_input-utf-16.txt

	./a.out unix_input-utf-16_swap.txt dst.txt -unix -win -swap
	cmp dst.txt win_input-utf-16.txt

	./a.out unix_input-utf-16_swap.txt dst.txt -unix -unix -swap
	cmp dst.txt unix_input-utf-16.txt

	./a.out win_input-utf-16_swap.txt dst.txt -win -mac -swap
	cmp dst.txt mac_input-utf-16.txt

	./a.out win_input-utf-16_swap.txt dst.txt -win -unix -swap
	cmp dst.txt unix_input-utf-16.txt

	./a.out win_input-utf-16_swap.txt dst.txt -win -win -swap
	cmp dst.txt win_input-utf-16.txt

	echo -e '\E[37;44m'"\033[1m/////// Testing//////\033[0m"

	./a.out mac_input-utf-16.txt 


	./a.out mac_input-utf-16.txt dst.txt -unix


	./a.out mac_input-utf-16.txt -win -win -swap


	./a.out  mac_input-utf-16.txt -win


	./a.out mac_input-utf-16.txt dst.txt -win -swap


	./a.out mac_input-utf-16.txt dst.txt -swap


	./a.out aaaaaaaa.txt dst.txt -win -unix -keep

	echo -e '\E[37;44m'"\033[1m/////// END//////\033[0m"
