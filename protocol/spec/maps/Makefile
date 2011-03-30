all: semantic_maps.pdf semantic_maps.html

semantic_maps.pdf: semantic_maps.toc semantic_maps.aux
	pdflatex semantic_maps.tex

semantic_maps.toc semantic_maps.aux: semantic_maps.tex atlas.sty
	pdflatex semantic_maps.tex

semantic_maps.html: semantic_maps.tex atlas.sty
	latex2html semantic_maps.tex
