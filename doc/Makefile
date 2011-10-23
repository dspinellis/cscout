#UXHOST=spiti
UXHOST=istlab
#DESTDIR="c:/tmp/cscout"
DESTDIR="c:/dds/pubs/web/home/cscout"
NOTETOOLS=/dds/pubs/courses/tools

.SUFFIXES:.java .dot .ps .gif .pic .eps .png

.java.dot:
	javadoc -docletpath ../lib/UmlGraph.jar -doclet gr.spinellis.umlgraph.doclet.UmlGraph -private $<
	mv graph.dot $@

.dot.eps:
	dot -Teps -o$@ $<

.dot.gif:
	dot -Tgif $< -o$@

.pic.eps:
	perl -p -e 'BEGIN {binmode(STDOUT);} s/\r//' $< | $(PIC2PLOT) -Teps >$@

all: mancscout.xml mancswc.xml dbschema.xml
	rm -f style.css
	cp ../refactor/style.css .
	chmod 666 style.css
	cat local.css >>style.css
	chmod 444 style.css
	sh /cygdrive/c$(NOTETOOLS)/coursebib.sh bib
	echo "The CScout Refactoring Browser" | perl $(NOTETOOLS)/thread.pl -h html -r -p ../index.html index ${DESTDIR}/doc
	cp simul.html ${DESTDIR}/doc
	cp logo.png ${DESTDIR}

mancscout.xml: ../man/cscout.1
	echo '<?xml version="1.0" ?>' >mancscout.xml
	echo '<notes>' >>mancscout.xml
	plink $(UXHOST) groff -mhtml -Thtml -man <../man/cscout.1 | sed -e '1,/<body>/d;/<\/body>/,$$d;s/&minus;/-/g;/<h1/d;s/&bull;/\&#8226;/g;s/&quot;/"/g' >>mancscout.xml
	echo '</notes>' >>mancscout.xml

mancswc.xml: ../man/cswc.1
	echo '<?xml version="1.0" ?>' >mancswc.xml
	echo '<notes>' >>mancswc.xml
	plink $(UXHOST) groff -mhtml -Thtml -man <../man/cswc.1 | sed -e '1,/<body>/d;/<\/body>/,$$d;s/&minus;/-/g;/<h1/d;s/&bull;/\&#8226;/g;s/&quot;/"/g' >>mancswc.xml
	echo '</notes>' >>mancswc.xml

dbschema.xml: ../refactor/workdb.cpp ../refactor/metrics.cpp ../refactor/docschema.pl
	perl ../refactor/docschema.pl ../refactor/workdb.cpp >dbschema.xml
clean:
	rm ${DESTDIR}/*
