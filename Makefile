include build/main.mk

test: $(DEFAULT_GOAL)
	test/diag_sanity_check.sh
