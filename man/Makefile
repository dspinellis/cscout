all: cscout.pdf cswc.pdf csmake.pdf cscc.pdf cscut.pdf cssplit.pdf \
	csreconst.pdf csmerge.pdf

%.pdf: %.1
	groff -Tps -man <$< >$$.ps
	ps2pdf $$.ps $@
	rm $$.ps
