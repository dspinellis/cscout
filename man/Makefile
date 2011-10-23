all: cscout.pdf cswc.pdf

%.pdf: %.1
	groff -Tps -man <$< >$$.ps
	ps2pdf $$.ps $@
	rm $$.ps
