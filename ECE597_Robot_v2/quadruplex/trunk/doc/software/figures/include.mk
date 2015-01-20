CLEANS += \
	doc/software/figures/leg-segments.svg \
	doc/software/figures/leg-segments.pdf

doc/software/figures/leg-segments.svg: doc/software/figures/leg-segments-template.svg doc/software/figures/leg-segments.py src/kinematics/leg_defs.py
	doc/software/figures/leg-segments.py $< $@

%.pdf: %.svg
	inkscape -z -C -d 300 --export-pdf="$@" "$<"

