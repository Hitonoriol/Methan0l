.PHONY: all pdf html clean

DOC_NAME = methan0l-reference
TMP_DOC = methan0l-tmp.md

all: pdf html

$(TMP_DOC):
	./pandoc/dyndown.sh $(DOC_NAME).md $@

pdf: % : $(DOC_NAME).%

$(DOC_NAME).pdf: $(TMP_DOC)
	pandoc -f markdown -t pdf \
	  --standalone \
	  --pdf-engine=xelatex \
	  --template=pandoc/eisvogel.latex \
	  --metadata-file=pandoc/meta.yaml \
	  --toc --number-sections \
	  --listings \
	  --highlight-style kate \
	  --indented-code-classes=java \
	  $(TMP_DOC) -o $@

html: % : $(DOC_NAME).%

$(DOC_NAME).html: $(TMP_DOC)
	pandoc -f markdown -t html5 \
	  --standalone --embed-resources \
	  --css=pandoc/panam.css \
	  --toc --number-sections \
	  --metadata-file=pandoc/meta.yaml \
	  $(TMP_DOC) -o $@

clean:
	rm -f $(DOC_NAME).pdf $(DOC_NAME).html $(TMP_DOC)