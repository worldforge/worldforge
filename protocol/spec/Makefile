CODE=gen_html.py ParseDef.py def2xml.py gen_atlas.py
DEFS=root.atlas entity.atlas operation.atlas type.atlas interface.atlas
OLD_DEFS=root.def entity.def operation.def type.def interface.def map.def agrilan_map.def
INPUT=root_in.html entity_in.html operation_in.html type_in.html \
	interface_in.html
HTML_OUTPUT=root.html entity.html operation.html type.html interface.html \
	map.html agrilan_map.html

all: atlas.atlas atlas.xml ${HTML_OUTPUT}

atlas.atlas: ${CODE} ${INPUT} ${DEFS}
	python gen_atlas.py

atlas.xml: ${CODE} ${INPUT} ${OLD_DEFS}
	python def2xml.py

${HTML_OUTPUT}: atlas.atlas ${CODE} ${INPUT} ${DEFS}
	python gen_html.py

