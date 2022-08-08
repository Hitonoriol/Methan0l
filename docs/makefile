.PHONY: all pdf html clean

DOC_NAME = methan0l-reference

all: pdf html

pdf: % : $(DOC_NAME).%

$(DOC_NAME).pdf:
	pandoc -f markdown -t pdf \
	  --standalone \
	  --pdf-engine=xelatex \
	  --template=pandoc/eisvogel.latex \
	  --metadata-file=pandoc/meta.yaml \
	  --toc --number-sections \
	  --listings \
	  --highlight-style kate \
	  --indented-code-classes=java \
	  $(DOC_NAME).md -o $@

html: % : $(DOC_NAME).%

$(DOC_NAME).html:
	pandoc -f markdown -t html5 \
	  --standalone --embed-resources \
	  --css=pandoc/panam.css \
	  --toc --number-sections \
	  --metadata-file=pandoc/meta.yaml \
	  $(DOC_NAME).md -o $@

clean:
	rm -f $(DOC_NAME).pdf $(DOC_NAME).html