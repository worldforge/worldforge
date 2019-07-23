CODE=gen_html.py ParseDef.py def2xml.py gen_atlas.py
DEFS=root.atlas entity.atlas operation.atlas type.atlas
OLD_DEFS=root.def entity.def operation.def type.def
INPUT=root_in.html entity_in.html operation_in.html type_in.html
HTML_OUTPUT=root.html entity.html operation.html type.html

all: atlas.atlas atlas.xml ${HTML_OUTPUT}

atlas.atlas: ${CODE} ${DEFS}
	python gen_atlas.py

atlas.xml: ${CODE} ${INPUT} ${OLD_DEFS}
	python def2xml.py

${HTML_OUTPUT}: atlas.atlas ${CODE} ${INPUT} ${DEFS}
	python gen_html.py

