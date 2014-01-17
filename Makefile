include build/main.mk

test: $(DEFAULT_GOAL)
	cd test && ./diag_sanity_test.sh
