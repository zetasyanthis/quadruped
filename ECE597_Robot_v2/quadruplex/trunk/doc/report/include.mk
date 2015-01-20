CLEANS += \
    doc/report/report.pdf

REPORT_FIGURES := \
    doc/report/figures/*.jpg

doc/report/report.pdf: doc/report/report.tex $(REPORT_FIGURES)
	cd doc/report && \
	pdflatex $(TEXFLAGS) report.tex && \
	bibtex report; \
	pdflatex $(TEXFLAGS) report.tex && \
	pdflatex $(TEXFLAGS) report.tex && \
	rm -f report.aux report.bbl report.blg report.brf report.log report.out

%.pdf: %.eps
	epstopdf %<

