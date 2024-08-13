test:
	! for i in *.rdbu; do rdbunit --database=sqlite $$i  | sqlite3 ; done | grep 'not ok'
	@echo All tests passed
