"""
Genera INFORME_DESTINO.docx actualizado para Skell's GreenHouse V3.0
Usa unpacked_original como base (estilos, imágenes, numeración ya presentes).
"""
import zipfile, os, re

BASE = r"C:\Users\Skellent\Projects\Personales\GreenHouse\prompts\unpacked_original"
OUTPUT = r"C:\Users\Skellent\Projects\Personales\GreenHouse\prompts\INFORME_DESTINO.docx"

# ── Helpers ────────────────────────────────────────────────────────────────

def p(text="", style=None, numId=None, ilvl=0, bold=False, spacing_before=0,
      spacing_after=0, align=None, shade=None, font="", size=0, color="",
      bookmark_id=None, bookmark_name=None, page_break_before=False,
      xml_space=False):
    """Returns a <w:p> XML string."""
    parts = []
    ppr_inner = []
    if style:
        ppr_inner.append(f'<w:pStyle w:val="{style}"/>')
    if numId is not None:
        ppr_inner.append(f'<w:numPr><w:ilvl w:val="{ilvl}"/><w:numId w:val="{numId}"/></w:numPr>')
    if spacing_before != 0 or spacing_after != 0:
        ppr_inner.append(f'<w:spacing w:before="{spacing_before}" w:after="{spacing_after}"/>')
    if shade:
        ppr_inner.append(f'<w:shd w:val="clear" w:color="auto" w:fill="{shade}"/>')
    if align:
        ppr_inner.append(f'<w:jc w:val="{align}"/>')

    if font or size or color:
        rpr_inner = []
        if font:
            rpr_inner.append(f'<w:rFonts w:ascii="{font}" w:hAnsi="{font}" w:cs="{font}"/>')
        if size:
            rpr_inner.append(f'<w:sz w:val="{size}"/><w:szCs w:val="{size}"/>')
        if color:
            rpr_inner.append(f'<w:color w:val="{color}"/>')
        ppr_inner.append(f'<w:rPr>{"".join(rpr_inner)}</w:rPr>')

    ppr = f'<w:pPr>{"".join(ppr_inner)}</w:pPr>' if ppr_inner else ''

    run_parts = []
    if bookmark_id is not None and bookmark_name:
        run_parts.append(f'<w:bookmarkStart w:id="{bookmark_id}" w:name="{bookmark_name}" w:colFirst="0" w:colLast="0"/>')
        run_parts.append(f'<w:bookmarkEnd w:id="{bookmark_id}"/>')

    if text:
        rpr = ''
        if bold or font or size or color:
            rpr_inner = []
            if bold:
                rpr_inner.append('<w:b/><w:bCs/>')
            if font:
                rpr_inner.append(f'<w:rFonts w:ascii="{font}" w:hAnsi="{font}" w:cs="{font}"/>')
            if size:
                rpr_inner.append(f'<w:sz w:val="{size}"/><w:szCs w:val="{size}"/>')
            if color:
                rpr_inner.append(f'<w:color w:val="{color}"/>')
            rpr = f'<w:rPr>{"".join(rpr_inner)}</w:rPr>'
        sp = ' xml:space="preserve"' if (xml_space or text.startswith(' ') or text.endswith(' ')) else ''
        pb = '<w:br w:type="page"/>' if page_break_before else ''
        run_parts.append(f'<w:r>{rpr}<w:t{sp}>{pb}{text}</w:t></w:r>')

    inner = ppr + ''.join(run_parts)
    return f'<w:p w14:paraId="AA{len(inner):06X}" w14:textId="77777777" w:rsidR="00917BB2" w:rsidRDefault="00000000">{inner}</w:p>\n'


def h1(text, bm_id, bm_name):
    return p(text, style="Ttulo1", bookmark_id=bm_id, bookmark_name=bm_name)

def h2(text, bm_id, bm_name):
    return p(text, style="Ttulo2", bookmark_id=bm_id, bookmark_name=bm_name)

def h3(text, bm_id, bm_name):
    return p(text, style="Ttulo3", bookmark_id=bm_id, bookmark_name=bm_name)

def h4(text, bm_id, bm_name):
    return p(text, style="Ttulo4", bookmark_id=bm_id, bookmark_name=bm_name)

def np(text, xml_space=True):
    """Normal paragraph."""
    sp = ' xml:space="preserve"' if xml_space else ''
    return f'<w:p w14:paraId="AA{hash(text)&0xFFFFFF:06X}" w14:textId="77777777" w:rsidR="00917BB2" w:rsidRDefault="00000000"><w:r><w:t{sp}>{text}</w:t></w:r></w:p>\n'

def bullet(bold_part, rest="", numId=1, ilvl=0):
    """Bullet or list item: bold_part in bold + rest in normal."""
    bp = f'<w:r><w:rPr><w:b/><w:bCs/></w:rPr><w:t xml:space="preserve">{bold_part}</w:t></w:r>' if bold_part else ''
    rp = f'<w:r><w:t xml:space="preserve">{rest}</w:t></w:r>' if rest else ''
    return (f'<w:p w14:paraId="BB{numId:02X}{hash(bold_part+rest)&0xFFFF:04X}" w14:textId="77777777" '
            f'w:rsidR="00917BB2" w:rsidRDefault="00000000">'
            f'<w:pPr><w:numPr><w:ilvl w:val="{ilvl}"/><w:numId w:val="{numId}"/></w:numPr>'
            f'<w:spacing w:before="0" w:after="0"/></w:pPr>'
            f'{bp}{rp}</w:p>\n')

def code(line):
    """A monospaced code-block line."""
    escaped = (line.replace('&', '&amp;').replace('<', '&lt;').replace('>', '&gt;'))
    sp = ' xml:space="preserve"' if line != line.strip() or True else ''
    return (f'<w:p w14:paraId="CC{hash(line)&0xFFFFFF:06X}" w14:textId="77777777" '
            f'w:rsidR="00917BB2" w:rsidRDefault="00000000">'
            f'<w:pPr><w:shd w:val="clear" w:color="auto" w:fill="F5F5F5"/>'
            f'<w:spacing w:before="0" w:after="0"/>'
            f'<w:rPr><w:rFonts w:ascii="Courier New" w:hAnsi="Courier New" w:cs="Courier New"/>'
            f'<w:sz w:val="16"/><w:szCs w:val="16"/></w:rPr></w:pPr>'
            f'<w:r><w:rPr><w:rFonts w:ascii="Courier New" w:hAnsi="Courier New" w:cs="Courier New"/>'
            f'<w:sz w:val="16"/><w:szCs w:val="16"/></w:rPr>'
            f'<w:t{sp}>{escaped}</w:t></w:r></w:p>\n')

def page_break():
    return '<w:p w14:paraId="DD000001" w14:textId="77777777" w:rsidR="00917BB2" w:rsidRDefault="00000000"><w:r><w:br w:type="page"/></w:r></w:p>\n'

def empty():
    return '<w:p w14:paraId="EE000001" w14:textId="77777777" w:rsidR="00917BB2" w:rsidRDefault="00000000"/>\n'

def image_para(rId, name, cx, cy, doc_id, align="center"):
    return f'''<w:p w14:paraId="FF{doc_id:06X}" w14:textId="77777777" w:rsidR="00917BB2" w:rsidRDefault="00000000">
<w:pPr><w:jc w:val="{align}"/></w:pPr>
<w:r><w:rPr><w:noProof/></w:rPr><w:drawing>
<wp:inline distT="114300" distB="114300" distL="114300" distR="114300" wp14:anchorId="{doc_id:08X}" wp14:editId="{doc_id+1:08X}">
<wp:extent cx="{cx}" cy="{cy}"/>
<wp:effectExtent l="0" t="0" r="0" b="0"/>
<wp:docPr id="{doc_id}" name="{name}"/>
<wp:cNvGraphicFramePr/>
<a:graphic xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
<a:graphicData uri="http://schemas.openxmlformats.org/drawingml/2006/picture">
<pic:pic xmlns:pic="http://schemas.openxmlformats.org/drawingml/2006/picture">
<pic:nvPicPr><pic:cNvPr id="0" name="{name}"/><pic:cNvPicPr preferRelativeResize="0"/></pic:nvPicPr>
<pic:blipFill><a:blip r:embed="{rId}"/><a:srcRect/><a:stretch><a:fillRect/></a:stretch></pic:blipFill>
<pic:spPr><a:xfrm><a:off x="0" y="0"/><a:ext cx="{cx}" cy="{cy}"/></a:xfrm>
<a:prstGeom prst="rect"><a:avLst/></a:prstGeom><a:ln/></pic:spPr>
</pic:pic></a:graphicData></a:graphic>
</wp:inline></w:drawing></w:r></w:p>\n'''

def toc_entry(text, anchor, indent=0):
    ind = f'<w:ind w:left="{indent}"/>' if indent else ''
    return (f'<w:p w14:paraId="T0{hash(text)&0xFFFF:04X}" w14:textId="77777777" '
            f'w:rsidR="00917BB2" w:rsidRDefault="00000000">'
            f'<w:pPr><w:widowControl w:val="0"/>'
            f'<w:tabs><w:tab w:val="right" w:pos="12000"/></w:tabs>'
            f'<w:spacing w:line="240" w:lineRule="auto"/>{ind}<w:jc w:val="left"/>'
            f'<w:rPr><w:sz w:val="22"/><w:szCs w:val="22"/><w:u w:val="single"/></w:rPr></w:pPr>'
            f'<w:hyperlink w:anchor="{anchor}">'
            f'<w:r><w:rPr><w:sz w:val="22"/><w:szCs w:val="22"/><w:u w:val="single"/></w:rPr>'
            f'<w:t>{text}</w:t><w:tab/><w:t>...</w:t></w:r></w:hyperlink></w:p>\n')

def hyperlink_ref(rId, url, pre="", post=""):
    pre_r = f'<w:r><w:rPr><w:lang w:val="en-US"/></w:rPr><w:t xml:space="preserve">{pre}</w:t></w:r>' if pre else ''
    link_r = (f'<w:hyperlink r:id="{rId}">'
              f'<w:r><w:rPr><w:color w:val="1155CC"/><w:u w:val="single"/></w:rPr>'
              f'<w:t>{url}</w:t></w:r></w:hyperlink>')
    post_r = f'<w:r><w:t xml:space="preserve">{post}</w:t></w:r>' if post else ''
    return (f'<w:p w14:paraId="RF{hash(url)&0xFFFFFF:06X}" w14:textId="77777777" '
            f'w:rsidR="00917BB2" w:rsidRDefault="00000000">'
            f'{pre_r}{link_r}{post_r}</w:p>\n')

# ── Cover page ─────────────────────────────────────────────────────────────

LOGO = f'''<w:p w14:paraId="10000001" w14:textId="77777777" w:rsidR="00917BB2" w:rsidRDefault="00000000">
<w:pPr><w:jc w:val="center"/><w:rPr><w:b/><w:bCs/></w:rPr></w:pPr>
<w:r><w:rPr><w:b/><w:bCs/><w:noProof/></w:rPr><w:drawing>
<wp:inline distT="114300" distB="114300" distL="114300" distR="114300" wp14:anchorId="390C7D67" wp14:editId="2F33B119">
<wp:extent cx="782475" cy="776008"/>
<wp:effectExtent l="0" t="0" r="0" b="0"/>
<wp:docPr id="6" name="image7.png"/>
<wp:cNvGraphicFramePr/>
<a:graphic xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
<a:graphicData uri="http://schemas.openxmlformats.org/drawingml/2006/picture">
<pic:pic xmlns:pic="http://schemas.openxmlformats.org/drawingml/2006/picture">
<pic:nvPicPr><pic:cNvPr id="0" name="image7.png"/><pic:cNvPicPr preferRelativeResize="0"/></pic:nvPicPr>
<pic:blipFill><a:blip r:embed="rId7"/><a:srcRect/><a:stretch><a:fillRect/></a:stretch></pic:blipFill>
<pic:spPr><a:xfrm><a:off x="0" y="0"/><a:ext cx="782475" cy="776008"/></a:xfrm>
<a:prstGeom prst="rect"><a:avLst/></a:prstGeom><a:ln/></pic:spPr>
</pic:pic></a:graphicData></a:graphic>
</wp:inline></w:drawing></w:r></w:p>\n'''

MEMBERS = (
    '<w:p w14:paraId="10000010" w14:textId="77777777" w:rsidR="00917BB2" w:rsidRDefault="00000000">'
    '<w:pPr><w:jc w:val="right"/></w:pPr>'
    '<w:r><w:rPr><w:b/><w:bCs/></w:rPr><w:t>Integrantes:</w:t></w:r>'
    '<w:r><w:rPr><w:color w:val="FFFFFF"/></w:rPr><w:t>--------------------------------</w:t></w:r></w:p>\n'
    '<w:p w14:paraId="10000011" w14:textId="77777777" w:rsidR="00917BB2" w:rsidRDefault="00000000">'
    '<w:pPr><w:numPr><w:ilvl w:val="0"/><w:numId w:val="11"/></w:numPr><w:jc w:val="right"/></w:pPr>'
    '<w:r><w:t>Robert Rodr&#xED;guez, V-32.942.787, 17 A&#xF1;os.</w:t></w:r>'
    '<w:r><w:rPr><w:color w:val="FFFFFF"/></w:rPr><w:t>----</w:t></w:r></w:p>\n'
    '<w:p w14:paraId="10000012" w14:textId="77777777" w:rsidR="00917BB2" w:rsidRDefault="00000000">'
    '<w:pPr><w:numPr><w:ilvl w:val="0"/><w:numId w:val="11"/></w:numPr><w:jc w:val="right"/></w:pPr>'
    '<w:r><w:t>Christopher Ram&#xED;rez, V-33.499.939, 16 A&#xF1;os.</w:t></w:r></w:p>\n'
    '<w:p w14:paraId="10000013" w14:textId="77777777" w:rsidR="00917BB2" w:rsidRDefault="00000000">'
    '<w:pPr><w:numPr><w:ilvl w:val="0"/><w:numId w:val="11"/></w:numPr><w:jc w:val="right"/></w:pPr>'
    '<w:r><w:t>Fabiana Hern&#xE1;ndez, V-34.648.529, 14 A&#xF1;os.</w:t></w:r>'
    '<w:r><w:rPr><w:color w:val="FFFFFF"/></w:rPr><w:t>-</w:t></w:r></w:p>\n'
    '<w:p w14:paraId="10000014" w14:textId="77777777" w:rsidR="00917BB2" w:rsidRDefault="00000000">'
    '<w:pPr><w:ind w:left="4820" w:firstLine="282"/><w:jc w:val="right"/></w:pPr>'
    '<w:r><w:rPr><w:b/><w:bCs/></w:rPr><w:t>Tutor:</w:t></w:r>'
    '<w:r><w:t xml:space="preserve"> Jes&#xFA;s P&#xE9;rez, V-30.759.313</w:t></w:r></w:p>\n'
)

COVER = (
    p("Olimpiadas de Rob\xF3tica Creativa (ORC)", bold=True, align="center") +
    p("COMPETENCIA INGENIO CREATIVO", bold=True, align="center") +
    p("INFORME T\xC9CNICO", bold=True, align="center") +
    empty() + empty() + empty() +
    LOGO +
    p("Skellent", bold=True, align="center") +
    p("Skell’s GreenHouse V3.0", bold=True, align="center") +
    empty() + empty() + empty() +
    MEMBERS +
    empty() +
    '<w:p w14:paraId="10000020" w14:textId="77777777" w:rsidR="00917BB2" w:rsidRDefault="00000000">'
    '<w:pPr><w:jc w:val="center"/></w:pPr>'
    '<w:r><w:t>Barquisimeto, Mayo 2026</w:t><w:br w:type="page"/></w:r></w:p>\n'
)

# ── TOC ───────────────────────────────────────────────────────────────────

TOC_HEADER = p("&#xCD;NDICE", bold=True, align="center")

TOC_ENTRIES = (
    toc_entry("RES\xDAMEN DE LA IDEA DE PROYECTO", "_res")
  + toc_entry("INTEGRANTES DEL EQUIPO DE PROYECTO", "_int")
  + toc_entry("IMPLEMENTACI\xD3N DEL PROYECTO", "_impl")
  + toc_entry("INICIO", "_inicio", indent=360)
  + toc_entry("DESARROLLO", "_des", indent=360)
  + toc_entry("PRINCIPALES DIFICULTADES Y SOLUCIONES", "_dif", indent=720)
  + toc_entry("MATERIALES DEFINITIVOS", "_mat", indent=720)
  + toc_entry("PROGRAMACI\xD3N", "_prog", indent=720)
  + toc_entry("ALICIA — INTELIGENCIA ARTIFICIAL", "_alicia", indent=720)
  + toc_entry("ASPECTOS MEC\xC1NICOS Y FUNCIONAMIENTO", "_mec", indent=720)
  + toc_entry("ALGORITMO DE CONTROL", "_algo", indent=360)
  + toc_entry("ESTRUCTURA DEL C\xD3DIGO", "_estr", indent=360)
  + toc_entry("EJECUCI\xD3N", "_ejec", indent=360)
  + toc_entry("EVALUACI\xD3N DEL DESEMPE\xD1O", "_eval_des", indent=720)
  + toc_entry("FORTALEZAS DEL SISTEMA", "_fort", indent=720)
  + toc_entry("OPORTUNIDADES DE MEJORA", "_opor", indent=720)
  + toc_entry("EVALUACI\xD3N Y AN\xC1LISIS DEL PROYECTO", "_eval")
  + toc_entry("LOGROS OBTENIDOS", "_logros", indent=360)
  + toc_entry("APRENDIZAJES", "_aprend", indent=360)
  + toc_entry("ESCENARIOS DE ESCALADO", "_escala", indent=360)
  + toc_entry("APLICACI\xD3N M\xD3VIL", "_app")
  + toc_entry("ESQUEMA ELECTR\xD3NICO", "_esq_elec")
  + toc_entry("ESQUEMA DE FLUJO EL\xC9CTRICO", "_esq_flu")
  + toc_entry("ANEXOS DE C\xD3DIGO", "_anexos")
  + toc_entry("REFERENCIAS BIBLIOGR\xC1FICAS", "_ref")
  + toc_entry("FOTOGRAF\xCDAS", "_fotos")
)

TOC_FIELD_START = (
    '<w:sdt><w:sdtPr><w:id w:val="387005718"/>'
    '<w:docPartObj><w:docPartGallery w:val="Table of Contents"/><w:docPartUnique/></w:docPartObj>'
    '</w:sdtPr><w:sdtContent>'
)
TOC_FIELD_END = '</w:sdtContent></w:sdt>\n'

TOC_BLOCK = (TOC_FIELD_START +
    '<w:p w14:paraId="T00001" w14:textId="77777777" w:rsidR="00917BB2" w:rsidRDefault="00000000">'
    '<w:pPr><w:widowControl w:val="0"/><w:tabs><w:tab w:val="right" w:pos="12000"/></w:tabs>'
    '<w:spacing w:line="240" w:lineRule="auto"/><w:jc w:val="left"/>'
    '<w:rPr><w:sz w:val="22"/><w:szCs w:val="22"/><w:u w:val="single"/></w:rPr></w:pPr>'
    '<w:r><w:fldChar w:fldCharType="begin"/>'
    '<w:instrText xml:space="preserve"> TOC \\h \\u \\z \\t "Heading 1,1,Heading 2,2,Heading 3,3"</w:instrText>'
    '<w:fldChar w:fldCharType="separate"/></w:r>'
    + TOC_ENTRIES +
    '<w:p w14:paraId="T00002" w14:textId="77777777" w:rsidR="00917BB2" w:rsidRDefault="00000000">'
    '<w:r><w:fldChar w:fldCharType="end"/></w:r></w:p>'
    + TOC_FIELD_END)

# ── Sections ───────────────────────────────────────────────────────────────

RESUMEN = (
    h1("RES\xDAMEN DE LA IDEA DE PROYECTO", 0, "_res") +
    np("Skell’s GreenHouse es un sistema de invernadero aut\xF3nomo, port\xE1til y modular, dise\xF1ado "
       "para facilitar el cultivo y la adaptaci\xF3n de plantas sensibles en condiciones ambientales "
       "desfavorables. Su prop\xF3sito central es reproducir el entorno ideal de cualquier planta "
       "— independientemente del clima externo — para inducir de forma gradual su adaptaci\xF3n "
       "al ambiente real, aumentando la producci\xF3n agr\xEDcola sin depender de temporadas ni zonas cl\xE1sicas.") +
    np("El coraz\xF3n del sistema es Alicia, una red neuronal perceptr\xF3n multicapa (MLP) entrenada en "
       "Python con TensorFlow y desplegada mediante TensorFlow Lite Micro sobre un ESP32-S3 con 8 MB de PSRAM "
       "Octal. Alicia procesa seis variables (temperatura del ambiente, temperatura del agua, humedad del "
       "ambiente, humedad del suelo, intensidad de luz y perfil de planta) y decide autom\xE1ticamente la "
       "intensidad de los LEDs UV y si se debe activar el riego. La arquitectura distribuida usa un ESP32-S3 "
       "como cerebro (IA + API HTTP + c\xE1mara OV2640) y un ESP32 com\xFAn como n\xF3do de sensores y "
       "actuadores; ambos se comunican mediante UART con protocolo JSON. Una pantalla LCD 16x2 I2C muestra "
       "en tiempo real los datos de los sensores y las decisiones de la IA.") +
    np("Las macetas est\xE1n equipadas individualmente con sensores FC-28, lo que permite intercambiarlas "
       "f\xE1cilmente sin recableado. El sistema funciona con 5 V / 3 A, con conversi\xF3n interna a 3.3 V. "
       "Una aplicaci\xF3n m\xF3vil desarrollada en JavaScript se conecta por WiFi a la IP local del ESP32-S3 "
       "y permite monitorear todos los par\xE1metros, ver la c\xE1mara en tiempo real y configurar el "
       "perfil de planta.") +
    np("El proyecto se enmarca en los Motores Agroalimentario y Ciudades Inteligentes. En el motor "
       "agroalimentario, GreenHouse permite cultivar hortalizas en cualquier zona clim\xE1tica mediante "
       "la recreaci\xF3n del entorno ideal y la posterior adaptaci\xF3n gradual, eliminando la "
       "dependencia de temporadas e impulsando la soberan\xEDa alimentaria. En el motor de Ciudades "
       "Inteligentes, representa una soluci\xF3n accesible para ciudadanos que desean sembrar en casa "
       "sin conocimientos agron\xF3micos ni tiempo para el cuidado manual. Las pruebas realizadas con "
       "ceboll\xEDn en el hogar de Robert Rodr\xEDguez demostraron crecimiento continuo, riego preciso "
       "y monitoreo confiable.")
)

INTEGRANTES = (
    h1("INTEGRANTES DEL EQUIPO DE PROYECTO", 1, "_int") +
    np("El equipo Skellent se form\xF3 a partir de la amistad y el inter\xE9s compartido por la rob\xF3tica y "
       "la electr\xF3nica. Robert y Christopher se conocen de proyectos previos; Robert y Fabiana estudian "
       "en el mismo colegio. Al identificar la problem\xE1tica del cuidado de plantas en sus hogares, "
       "decidieron unir sus habilidades para crear una soluci\xF3n tecnol\xF3gica real. Cada integrante "
       "asumi\xF3 roles espec\xEDficos basados en sus fortalezas:") +
    bullet("Robert Rodr\xEDguez (17 a\xF1os):", " Programaci\xF3n del ESP32-S3 y ESP32 Nervius, "
           "entrenamiento de la red neuronal Alicia en Python, desarrollo de la aplicaci\xF3n m\xF3vil "
           "e integraci\xF3n de TFLite Micro.", numId=6) +
    bullet("Christopher Ram\xEDrez (16 a\xF1os):", " Dise\xF1o mec\xE1nico, electr\xF3nico y "
           "estructural. Encargado del dise\xF1o de la carcasa, el sistema modular de macetas y la "
           "impresi\xF3n 3D de componentes.", numId=6) +
    bullet("Fabiana Hern\xE1ndez (14 a\xF1os):", " Electr\xF3nica y documentaci\xF3n. Supervis\xF3 "
           "el conexionado de sensores y actuadores, gestion\xF3 las pruebas y el registro fotogr\xE1fico.", numId=6)
)

IMPLEMENTACION = (
    h1("IMPLEMENTACI\xD3N DEL PROYECTO", 2, "_impl") +
    h2("INICIO", 3, "_inicio") +
    np("La idea de Skell’s GreenHouse naci\xF3 de un proyecto anterior a menor escala: un medidor "
       "de datos de maceta. Al observar que plantas en sus hogares mor\xEDan por falta de atenci\xF3n "
       "o por condiciones desfavorables, los integrantes ampliaron el concepto hacia un sistema "
       "aut\xF3nomo completo. Antes de iniciar, investigaron invernaderos comerciales — costosos, "
       "no port\xE1tiles, sin IA ni monitoreo remoto — y trabajos acad\xE9micos sobre agricultura "
       "de precisi\xF3n con ESP32, lo que confirm\xF3 la viabilidad del enfoque.") +
    np("Al inicio los materiales eran limitados: una placa ESP32, sensores b\xE1sicos, una bomba "
       "y cart\xF3n para el primer boceto. La evoluci\xF3n hasta V3.0 requiri\xF3 agregar el "
       "ESP32-S3 con c\xE1mara, distribuir la arquitectura en dos microcontroladores y dise\xF1ar "
       "el sistema de macetas modulares con FC-28 integrado.") +

    h2("DESARROLLO", 4, "_des") +
    np("El desarrollo se organiz\xF3 en fases:") +
    bullet("", "Definici\xF3n de requerimientos.", numId=8) +
    bullet("", "Dise\xF1o conceptual y selecci\xF3n de componentes.", numId=8) +
    bullet("", "Prototipado electr\xF3nico.", numId=8) +
    bullet("", "Construcci\xF3n mec\xE1nica.", numId=8) +
    bullet("", "Programaci\xF3n y pruebas.", numId=8) +
    bullet("", "Generaci\xF3n del dataset y entrenamiento de Alicia.", numId=8) +
    bullet("", "Iteraci\xF3n final e integraci\xF3n.", numId=8) +
    np("Se realizaron dos iteraciones estructurales: una versi\xF3n con cart\xF3n para validar medidas "
       "y la versi\xF3n final con PVC expandido y marcos de aluminio. La tercera gran iteraci\xF3n "
       "(V3.0) introdujo la arquitectura de dos ESP32, la red neuronal Alicia y las macetas "
       "con FC-28 integrado.") +

    h3("PRINCIPALES DIFICULTADES Y SOLUCIONES", 5, "_dif") +
    bullet("Sincronizaci\xF3n UART:", " Coordinar el ciclo de lectura del Nervius con el "
           "ciclo de inferencia del Core requiri\xF3 definir un protocolo JSON delimitado por "
           "salto de l\xEDnea y buffers protegidos por mutex FreeRTOS.", numId=5) +
    bullet("Memoria en el ESP32-S3:", " TFLite Micro, la c\xE1mara OV2640 y el servidor "
           "HTTP comparten la PSRAM. Se asign\xF3 el tensor arena de 16 KB en PSRAM "
           "(MALLOC_CAP_SPIRAM) mediante heap_caps_malloc(), liberando la RAM interna para el "
           "servidor.", numId=5) +
    bullet("Configuraci\xF3n del TFLite:", " Se utiliz\xF3 la librer\xEDa TensorFlow Lite Micro "
           "para ESP32. Requiri\xF3 m\xFAltiples pruebas para estabilizar la resoluci\xF3n del "
           "resolver de operadores y lograr la inferencia correcta.", numId=5) +
    bullet("Suministro de Agua:", " Inicialmente la bomba no succ\xEDona bien. Se reubiqu\xF3 "
           "en la parte baja del tanque y se aument\xF3 la altura de la manguera.", numId=5) +
    bullet("Modularidad:", " Las macetas llevan el FC-28 integrado, lo que permite "
           "intercambiarlas sin desconectar cables internos. El dise\xF1o mec\xE1nico y "
           "electr\xF3nico prioriza el f\xE1cil acceso y reemplazo de componentes.", numId=5) +

    h3("MATERIALES DEFINITIVOS", 6, "_mat") +
    bullet("Estructura:", " L\xE1minas de PVC expandido, marcos de aluminio, "
           "piezas impresas en 3D (PLA-PETG).", numId=13) +
    bullet("Electr\xF3nica:", " ESP32-S3 + OV2640 (8 MB PSRAM Octal), ESP32 com\xFAn (Nervius), "
           "sensor DHT22 (temperatura y humedad del aire), FC-28 \xD72 (humedad de suelo), "
           "HC-SR04 ultras\xF3nico (altura de planta), fotoresistencia (luz exterior), "
           "bomba de agua DC 5 V, LEDs UV \xD72, pantalla LCD 16x2 I2C (0x27), "
           "fuente conmutada 5 V 3 A, m\xF3dulo reductor LM2596 (3.3 V), "
           "bot\xF3n, baquelita, resistencias, transistores.", numId=13) +
    bullet("Conexiones:", " Cables DuPont, conectores terminales para cargas de alto consumo.", numId=13) +

    h3("PROGRAMACI\xD3N", 7, "_prog") +
    np("Se utiliz\xF3 Arduino C++ en el Arduino IDE tanto para el ESP32-S3 (core.ino) como para el "
       "ESP32 Nervius (nervius.ino), aprovechando las librer\xEDas WiFi, WebServer, ArduinoJson, "
       "LiquidCrystal_I2C y TensorFlow Lite Micro para Espressif. FreeRTOS gestiona las dos tareas "
       "paralelas: el bucle principal en n\xFAcleo 1 (lectura UART + inferencia IA) y el servidor "
       "HTTP en n\xFAcleo 0.") +
    np("La red neuronal Alicia se cre\xF3 con Python usando TensorFlow/Keras: un script genera el "
       "dataset sint\xE9tico (generar_datos.py), otro entrena el modelo (entrenar_alicia.py) y un "
       "tercero lo convierte a TFLite e incluye el array de bytes como cabecera C (convertir_alicia.py). "
       "La aplicaci\xF3n m\xF3vil se desarroll\xF3 en JavaScript, consumiendo la API JSON del ESP32-S3.") +

    h3("ALICIA — INTELIGENCIA ARTIFICIAL", 8, "_alicia") +
    np("Alicia es una red neuronal perceptr\xF3n multicapa (MLP) dise\xF1ada para correr "
       "en microcontrolador con recursos limitados. Su arquitectura es:") +
    bullet("Capa de entrada:", " 6 neuronas — temp_ambiente, temp_agua, hum_ambiente, "
           "hum_suelo, intensidad_luz, perfil_planta (todas normalizadas a [0, 1]).", numId=5) +
    bullet("Capa oculta 1:", " 16 neuronas, activaci\xF3n ReLU.", numId=5) +
    bullet("Capa oculta 2:", " 8 neuronas, activaci\xF3n ReLU.", numId=5) +
    bullet("Capa de salida:", " 2 neuronas, activaci\xF3n Sigmoid — salida[0] \xD7 255 "
           "= intensidad UV (uint8), salida[1] > 0.5 = activar riego (bool).", numId=5) +
    bullet("Total de par\xE1metros:", " 266 (pesos + sesgos).", numId=5) +
    np("El entrenamiento se realiz\xF3 con 5.000 muestras sint\xE9ticas generadas por "
       "generar_datos.py con l\xF3gica determinista: “mientras m\xE1s luz exterior, mayor "
       "intensidad UV para forzar fototropismo desde arriba”; el riego depende de umbrales de "
       "humedad de suelo y ambiente seg\xFAn el perfil de planta (des\xE9rtico, regular, alpino). "
       "Se a\xF1ade ruido gaussiano para robustez.") +
    np("Configuraci\xF3n de entrenamiento: optimizador Adam (lr = 0.001), p\xE9rdida MSE, "
       "m\xE1ximo 300 \xE9pocas, EarlyStopping (paciencia 15) y ReduceLROnPlateau (factor 0.5, "
       "paciencia 8). Divisi\xF3n 80/20 entrenamiento/validaci\xF3n.") +
    np("Para el despliegue, el modelo se convierte a formato TFLite con cuantizaci\xF3n din\xE1mica "
       "(Optimize.DEFAULT), reduci\xE9ndolo a ~4–8 KB. Se exporta como array de bytes en "
       "alicia_model.h. En el ESP32-S3, TFLite Micro carga el modelo usando un tensor arena de "
       "16 KB reservado en PSRAM, registrando solo las operaciones necesarias (FullyConnected, "
       "Relu, Logistic) para minimizar el uso de memoria.") +

    h3("ASPECTOS MEC\xC1NICOS Y FUNCIONAMIENTO", 9, "_mec") +
    np("El sistema se divide electr\xF3nicamente en dos n\xF3dos:") +
    bullet("ESP32-S3 (Core):", " Cerebro del sistema. Corre la IA Alicia en n\xFAcleo 1, "
           "sirve la API HTTP y la c\xE1mara en n\xFAcleo 0, gestiona la pantalla LCD 16x2 "
           "y recibe el bot\xF3n de configuraci\xF3n.", numId=3) +
    bullet("ESP32 Com\xFAn (Nervius):", " N\xF3do de sensores y actuadores. Lee DHT22, "
           "FC-28 \xD72, HC-SR04 y fotoresistencia; recibe las \xF3rdenes de UV (PWM LEDC) "
           "y riego (digital) y las ejecuta f\xEDsicamente.", numId=3) +
    np("Mec\xE1nica-estructuralmente se divide en:") +
    bullet("Marco de aluminio:", " Estructura base que da fuerza y estabilidad.", numId=3) +
    bullet("Placas de PVC expandido:", " A\xEDslan los componentes y la planta del exterior, "
           "protegi\xE9ndolos del agua, luz y calor.", numId=3) +
    bullet("", "Puerta de observaci\xF3n con sistema de imanes.", numId=3) +
    bullet("Macetas modulares:", " Cada maceta lleva integrado su FC-28, permitiendo "
           "intercambiarla sin recableado.", numId=3) +

    h2("ALGORITMO DE CONTROL", 10, "_algo") +
    bullet("El Nervius (ESP32 com\xFAn) cada 2 segundos:", "", numId=16) +
    bullet("", "Lee sensores (DHT22, FC-28 \xD72, HC-SR04, fotoresistencia).", numId=16, ilvl=1) +
    bullet("", "Env\xEDa JSON por UART: [temp_amb, temp_agua, hum_amb, hum_suelo, luz, ultrasonido].", numId=16, ilvl=1) +
    bullet("", "Recibe JSON de respuesta: [uv(0-255), riego(bool), ventilacion(0)].", numId=16, ilvl=1) +
    bullet("", "Ejecuta actuadores: ledcWrite(UV_LEDC_CANAL, uv) y digitalWrite(RIEGO_PIN, riego).", numId=16, ilvl=1) +
    bullet("El Core (ESP32-S3, N\xFAcleo 1) en su bucle principal:", "", numId=16) +
    bullet("", "Lee el JSON entrante por UART y actualiza la estructura Sensores (con mutex).", numId=16, ilvl=1) +
    bullet("", "Llama a Alicia::inferir() con los datos del sensor.", numId=16, ilvl=1) +
    bullet("Control de Actuadores:", " Escala la salida[0] \xD7 255 = uv_out (uint8), "
           "salida[1] > 0.5 = riego_out. Env\xEDa resultados al Nervius por UART.", numId=16, ilvl=1) +
    bullet("", "Actualiza la pantalla LCD con los sensores y las decisiones de la IA.", numId=16, ilvl=1) +
    bullet("El API HTTP (N\xFAcleo 0) en paralelo:", "", numId=16) +
    bullet("", "Gestiona la conexi\xF3n WiFi de forma independiente.", numId=16, ilvl=1) +
    bullet("", "Expone GET /api/datos (JSON), GET /api/foto (JPEG OV2640), "
           "POST /api/perfil y POST /api/wifi.", numId=16, ilvl=1) +

    h2("ESTRUCTURA DEL C\xD3DIGO", 11, "_estr") +
    bullet("core.ino:", numId=4) +
    bullet("", "setup(): inicializa LCD, pines, UART, WiFi, c\xE1mara, Alicia y "
           "delega ServidorWeb() a n\xFAcleo 0 con xTaskCreatePinnedToCore.", numId=4, ilvl=1) +
    bullet("", "loop(): lee JSON UART del Nervius, llama Alicia::inferir(), "
           "responde por UART y actualiza LCD.", numId=4, ilvl=1) +
    bullet("", "servidorWeb(): bucle infinito en n\xFAcleo 0, maneja peticiones HTTP "
           "y sirve JSON de sensores y captura de c\xE1mara.", numId=4, ilvl=1) +
    bullet("nervius.ino:", numId=4) +
    bullet("", "Configura LEDC (PWM para LEDs UV) y pines de actuadores.", numId=4, ilvl=1) +
    bullet("", "Lee sensores cada 2 s, env\xEDa JSON al Core y ejecuta los "
           "actuadores seg\xFAn la respuesta.", numId=4, ilvl=1) +
    bullet("alicia.h:", numId=4) +
    bullet("", "Encapsula Alicia::init() (carga del modelo en PSRAM) y "
           "Alicia::inferir() (normalizaci\xF3n + Invoke() + post-procesado).", numId=4, ilvl=1) +
    bullet("Scripts Python (directorio alicia/):", numId=4) +
    bullet("", "generar_datos.py → entrenar_alicia.py → convertir_alicia.py. "
           "Flujo completo de generaci\xF3n de dataset, entrenamiento y exportaci\xF3n a alicia_model.h.", numId=4, ilvl=1) +

    h2("EJECUCI\xD3N", 12, "_ejec") +
    np("El prototipo V3.0 se prob\xF3 en el hogar de Robert Rodr\xEDguez. Se instal\xF3 una maceta "
       "con ceboll\xEDn en crecimiento. La red neuronal Alicia tom\xF3 el control autom\xE1tico: "
       "ajust\xF3 la intensidad UV en funci\xF3n de la luz exterior y activ\xF3 el riego seg\xFAn "
       "la humedad del suelo y el perfil configurado. Los resultados mostraron crecimiento continuo "
       "y saludable, con el ceboll\xEDn desarrollando ramas m\xE1s gruesas durante su crecimiento, "
       "consistente con la mutaci\xF3n de adaptaci\xF3n buscada.") +

    h3("EVALUACI\xD3N DEL DESEMPE\xD1O", 13, "_eval_des") +
    np("El sistema se consider\xF3 \xF3ptimo cuando se cumplieron tres criterios:") +
    bullet("Crecimiento Continuo:", " la planta demostr\xF3 un crecimiento continuo, saludable "
           "y con desarrollo mec\xE1nico m\xE1s robusto que en condiciones naturales.", numId=14) +
    bullet("Precisi\xF3n en UV y Riego:", " Alicia activ\xF3 la bomba solo cuando la humedad "
           "del suelo descendi\xF3 por debajo del umbral del perfil activo, sin inundaciones ni "
           "sequ\xEDas, y regul\xF3 los LEDs UV con intensidad proporcional a la luz exterior.", numId=14) +
    bullet("Confiabilidad de la aplicaci\xF3n:", " los datos mostrados en la app coincid\xEDan "
           "con mediciones manuales realizadas con instrumentos de referencia.", numId=14) +

    h3("FORTALEZAS DEL SISTEMA", 14, "_fort") +
    bullet("Autonomía Total mediante la IA:", " no requiere intervenci\xF3n humana para mantener "
           "las condiciones adecuadas (solo recarga ocasional del tanque de agua).", numId=1) +
    bullet("Perfiles configurables:", " el usuario puede seleccionar el perfil de planta "
           "(des\xE9rtico, regular, alpino) desde la app o el bot\xF3n, y Alicia adapta su "
           "comportamiento autom\xE1ticamente.", numId=1) +
    bullet("Modularidad:", " permite cambiar de cultivo f\xE1cilmente; dise\xF1ado "
           "electr\xF3nica y estructuralmente para ser reparable y escalable.", numId=1) +
    bullet("Monitoreo Remoto:", " la app facilita el monitoreo desde cualquier dispositivo "
           "en la misma red WiFi.", numId=1) +
    bullet("Portabilidad:", " tama\xF1o y peso permiten trasladarlo sin dificultad.", numId=1) +

    h3("OPORTUNIDADES DE MEJORA", 15, "_opor") +
    bullet("", "Implementar sensores de pH y nutrientes para decisiones m\xE1s finas.", numId=15) +
    bullet("", "A\xF1adir un sistema de recollecci\xF3n de agua mediante placas Peltier.", numId=15) +
    bullet("", "Integrar el DS18B20 para medir temperatura real del agua.", numId=15) +
    bullet("", "Desarrollar una base de datos de plantas con par\xE1metros predefinidos.", numId=15) +
    bullet("", "Optimizar el consumo energ\xE9tico mediante modos de suspensi\xF3n del "
           "ESP32.", numId=15)
)

EVALUACION = (
    h1("EVALUACI\xD3N Y AN\xC1LISIS DEL PROYECTO", 16, "_eval") +
    np("Skell’s GreenHouse atiende simult\xE1neamente dos motores productivos. En el Motor "
       "Agroalimentario, la capacidad del sistema para recrear cualquier entorno ideal permite "
       "cultivar hortalizas y plantas sensibles en zonas clim\xE1ticamente inapropiadas, "
       "reduciendo la dependencia de temporadas y ampliando la superficie cultivable efectiva "
       "sin requerir hect\xE1reas de tierra. Al arriesgar un brote para conseguir una l\xEDnea "
       "adaptada, se obtiene material gen\xE9tico resistente al entorno local, con impacto "
       "directo en la seguridad alimentaria del estado Lara.") +
    np("En el Motor de Ciudades Inteligentes, GreenHouse es una soluci\xF3n accesible para "
       "ciudadanos comunes que desean cultivar en casa sin conocimientos agron\xF3micos ni "
       "tiempo disponible para el cuidado manual. Su dise\xF1o compacto, modular y aut\xF3nomo "
       "encaja en apartamentos, oficinas y espacios educativos, fomentando la alfabetizaci\xF3n "
       "tecnol\xF3gica y agr\xEDcola desde el nivel dom\xE9stico.") +

    h2("LOGROS OBTENIDOS", 17, "_logros") +
    bullet("", "Se consolid\xF3 un equipo interdisciplinario con roles claros y comunicaci\xF3n "
           "efectiva entre programaci\xF3n, electr\xF3nica y mec\xE1nica.", numId=9) +
    bullet("", "Se logr\xF3 un prototipo completamente funcional: dos ESP32 coordinados "
           "por UART, IA corriendo en tiempo real en PSRAM y aplicaci\xF3n m\xF3vil operativa.", numId=9) +
    bullet("", "Alicia aprendi\xF3 satisfactoriamente la l\xF3gica de control UV/riego "
           "y la generaliza ante entradas reales no vistas durante el entrenamiento.", numId=9) +
    bullet("", "Se document\xF3 el proceso completo, incluyendo gu\xEDa de entrenamiento "
           "(GUIA_ALICIA.md) y scripts de Python.", numId=9) +

    h2("APRENDIZAJES", 18, "_aprend") +
    bullet("", "La modularidad es cr\xEDtica: facilita pruebas, mantenimiento y "
           "escalabilidad futura.", numId=10) +
    bullet("", "El manejo de memoria en sistemas embebidos requiere planificaci\xF3n: "
           "la separaci\xF3n de tareas entre PSRAM y RAM interna fue clave para que "
           "c\xE1mara, IA y servidor coexistan.", numId=10) +
    bullet("", "El entrenamiento sint\xE9tico con l\xF3gica determinista permite crear "
           "datasets etiquetados sin necesidad de experimentos reales, acelerando el "
           "ciclo de desarrollo.", numId=10) +
    bullet("", "La separaci\xF3n en dos microcontroladores simplifica el c\xF3digo de "
           "cada uno y hace el sistema m\xE1s robusto ante fallos individuales.", numId=10) +

    h2("ESCENARIOS DE ESCALADO", 19, "_escala") +
    bullet("", "Escala educativa: versi\xF3n en kit de componentes para escuelas, "
           "con manuales did\xE1cticos.", numId=12) +
    bullet("", "Escala comunal: m\xF3dulo m\xE1s grande para cultivo de m\xFAltiples "
           "macetas en espacios comunitarios.", numId=12) +
    bullet("", "Escala comercial: reducci\xF3n de costos mediante dise\xF1o optimizado "
           "para producci\xF3n en serie, manteniendo la modularidad.", numId=12)
)

APP_MOVIL = (
    h1("APLICACI\xD3N M\xD3VIL", 20, "_app") +
    np("Se desarroll\xF3 una aplicaci\xF3n m\xF3vil en JavaScript que se conecta mediante WiFi "
       "local a la IP del ESP32-S3. La aplicaci\xF3n permite:") +
    bullet("", "Monitorear en tiempo real todos los sensores (temperatura, humedad, "
           "luz, ultrasonido, estado de la c\xE1mara).", numId=1) +
    bullet("", "Ver las decisiones actuales de Alicia (intensidad UV y estado del riego).", numId=1) +
    bullet("", "Visualizar capturas de la c\xE1mara OV2640 en tiempo real.", numId=1) +
    bullet("", "Configurar el perfil de planta (1=Des\xE9rtico, 2=Regular, 3=Alpino) "
           "mediante POST /api/perfil.", numId=1) +
    bullet("", "Cambiar las credenciales WiFi del ESP32-S3 mediante POST /api/wifi.", numId=1)
)

ESQ_ELEC = (
    h1("ESQUEMA ELECTR\xD3NICO", 21, "_esq_elec") +
    image_para("rId8", "image2.jpg", 5063701, 3562734, 10)
)

ESQ_FLU = (
    h1("ESQUEMA DE FLUJO EL\xC9CTRICO", 22, "_esq_flu") +
    image_para("rId9", "image3.jpg", 5186363, 3006712, 11)
)

# ── Code Annexes ──────────────────────────────────────────────────────────

LOOP_CODE = [
    "void loop() {",
    "  neopixelWrite(PIN_RGB, 0, 0, 255); // Indicador de iteración",
    "  actualizarLCD();",
    "  bufferEntrante = \"\";",
    "",
    "  // Lee JSON entrante por UART (enviado por el Nervius)",
    "  while (nervios.available()) {",
    "    char c = nervios.read();",
    "    if (c == '\\n') {",
    "      JsonDocument datos;",
    "      if (!deserializeJson(datos, bufferEntrante)) {",
    "        if (xSemaphoreTake(mutexDatos, pdMS_TO_TICKS(100)) == pdTRUE) {",
    "          sensores.sen_temperatura_ambiente = datos[0];",
    "          sensores.sen_temperatura_agua     = datos[1];",
    "          sensores.sen_humedad_ambiente     = datos[2];",
    "          sensores.sen_humedad_suelo        = datos[3];",
    "          sensores.sen_intensidad_luz       = datos[4];",
    "          sensores.sen_ultrasonido          = datos[5];",
    "          xSemaphoreGive(mutexDatos);",
    "        }",
    "        // ── Ejecución de Alicia ──",
    "        int  uv_ia = 0; bool riego_ia = false;",
    "        if (Alicia::inferir(sensores, uv_ia, riego_ia)) {",
    "          sensores.ia_intensidad_uv = uv_ia;",
    "          sensores.ia_riego        = riego_ia ? 1 : 0;",
    "          ejecutarActuadores(uv_ia, riego_ia, 0);",
    "        } else {",
    "          ejecutarActuadores(0, false, 0); // fallback seguro",
    "        }",
    "      }",
    "      bufferEntrante = \"\";",
    "    } else { bufferEntrante += c; }",
    "  }",
    "  vTaskDelay(pdMS_TO_TICKS(200));",
    "}",
]

INFERIR_CODE = [
    "bool inferir(const Sensores& s, int& uv_out, bool& riego_out) {",
    "  if (!_listo) return false;",
    "  // Normalizar las 6 entradas a [0, 1]",
    "  _input->data.f[0] = _norm(s.sen_temperatura_ambiente, -10.0f, 50.0f);",
    "  _input->data.f[1] = _norm(s.sen_temperatura_agua,      5.0f, 45.0f);",
    "  _input->data.f[2] = _norm(s.sen_humedad_ambiente,      0.0f,100.0f);",
    "  _input->data.f[3] = _norm(s.sen_humedad_suelo,         0.0f,100.0f);",
    "  _input->data.f[4] = _norm(s.sen_intensidad_luz,        0.0f,100.0f);",
    "  _input->data.f[5] = _norm((float)s.ia_perfilplanta,    1.0f,  3.0f);",
    "  // Ejecutar la inferencia",
    "  if (_interpreter->Invoke() != kTfLiteOk) return false;",
    "  // Escalar salidas: [0]=UV (float→uint8), [1]=riego (sigmoid > 0.5)",
    "  uv_out    = (int)(_output->data.f[0] * 255.0f);",
    "  riego_out = _output->data.f[1] > 0.5f;",
    "  return true;",
    "}",
]

PYTHON_CODE = [
    "def calcular_uv(luz, perfil, temp_amb):",
    "    multiplicadores = {1: 1.15, 2: 1.0, 3: 0.75}",
    "    uv = luz * 2.55 * multiplicadores[perfil]",
    "    if temp_amb > 35.0:",
    "        uv *= 0.85                       # alivio térmico",
    "    if luz < 15.0:                       # mínimo nocturno",
    "        minimos = {1: 40, 2: 30, 3: 20}",
    "        uv = max(uv, float(minimos[perfil]))",
    "    return int(np.clip(uv, 0, 255))",
    "",
    "def calcular_riego(temp_amb, temp_agua, hum_amb, hum_suelo, perfil):",
    "    if temp_agua < 8.0 or temp_agua > 40.0:",
    "        return False                     # proteger raíces",
    "    config = {",
    "        1: {'seco':25, 'humedo':50, 'amb_seco':25, 'temp_calor':35},",
    "        2: {'seco':40, 'humedo':65, 'amb_seco':40, 'temp_calor':30},",
    "        3: {'seco':55, 'humedo':80, 'amb_seco':55, 'temp_calor':25},",
    "    }[perfil]",
    "    if hum_suelo < config['seco']:   return True",
    "    if hum_suelo > config['humedo']: return False",
    "    if hum_amb   < config['amb_seco']:    return True",
    "    if temp_amb  > config['temp_calor']:  return True",
    "    return False",
]

ANEXOS = (
    page_break() +
    h1("ANEXOS DE C\xD3DIGO", 23, "_anexos") +
    h2("Anexo A — loop() en core.ino (ESP32-S3, N\xFAcleo 1)", 24, "_anex_a") +
    np("Bucle principal que recibe datos por UART del Nervius, ejecuta Alicia y "
       "devuelve las decisiones:") +
    ''.join(code(l) for l in LOOP_CODE) +
    empty() +
    h2("Anexo B — inferir() en alicia.h", 25, "_anex_b") +
    np("Funci\xF3n que normaliza las entradas, ejecuta el intérprete TFLite y "
       "post-procesa las salidas:") +
    ''.join(code(l) for l in INFERIR_CODE) +
    empty() +
    h2("Anexo C — L\xF3gica determinista en generar_datos.py (Python)", 26, "_anex_c") +
    np("Funciones que definen las reglas que Alicia aprende durante el entrenamiento:") +
    ''.join(code(l) for l in PYTHON_CODE)
)

REFERENCIAS = (
    page_break() +
    h1("REFERENCIAS BIBLIOGR\xC1FICAS", 27, "_ref") +
    p("Fundamentos de Invernaderos Inteligentes y Agricultura de Precisi\xF3n",
      style="Ttulo4", numId=2, ilvl=0, bold=True, bookmark_id=28, bookmark_name="_idc7o3wm4q1r") +
    ('<w:p w14:paraId="RE000001" w14:textId="77777777" w:rsidR="00917BB2" w:rsidRDefault="00000000">'
     '<w:r><w:rPr><w:lang w:val="en-US"/></w:rPr>'
     '<w:t xml:space="preserve">Heuvelink, E., Hemming, S., &amp; Marcelis, L. F. M. (2025). '
     'Some recent developments in controlled-environment agriculture. '
     'The Journal of Horticultural Science and Biotechnology, 100(5), 604–614. </w:t></w:r>'
     '<w:hyperlink r:id="rId18"><w:r><w:rPr><w:color w:val="1155CC"/><w:u w:val="single"/></w:rPr>'
     '<w:t>https://doi.org/10.1080/14620316.2024.2440592</w:t></w:r></w:hyperlink></w:p>\n') +
    ('<w:p w14:paraId="RE000002" w14:textId="77777777" w:rsidR="00917BB2" w:rsidRDefault="00000000">'
     '<w:r><w:rPr><w:lang w:val="en-US"/></w:rPr>'
     '<w:t xml:space="preserve">Kim, J., Lee, H., &amp; Park, S. (2025). '
     'Multi-Sensor Monitoring, Intelligent Control, and Data Processing for Smart Greenhouse '
     'Environment Management. Sensors, 25(19), 6134. </w:t></w:r>'
     '<w:hyperlink r:id="rId19"><w:r><w:rPr><w:color w:val="1155CC"/><w:u w:val="single"/></w:rPr>'
     '<w:t>https://doi.org/10.3390/s25196134</w:t></w:r></w:hyperlink></w:p>\n') +
    p("Internet de las Cosas (IoT) y Monitoreo Remoto", style="Ttulo4", numId=2, ilvl=0, bold=True,
      bookmark_id=29, bookmark_name="_6aifamxa59p9") +
    ('<w:p w14:paraId="RE000003" w14:textId="77777777" w:rsidR="00917BB2" w:rsidRDefault="00000000">'
     '<w:r><w:t xml:space="preserve">Kim, J., Lee, H., &amp; Park, S. (2025). '
     'Multi-Sensor Monitoring, Intelligent Control, and Data Processing for Smart Greenhouse '
     'Environment Management. Sensors, 25(19), 6134. </w:t></w:r>'
     '<w:hyperlink r:id="rId22"><w:r><w:rPr><w:color w:val="1155CC"/><w:u w:val="single"/></w:rPr>'
     '<w:t>https://doi.org/10.3390/s25196134</w:t></w:r></w:hyperlink>'
     '<w:r><w:t xml:space="preserve">  (Discute protocolos WiFi y plataformas IoT para '
     'automatizaci\xF3n.)</w:t></w:r></w:p>\n')
)

# Photos section (all from original)
FOTOS = (
    page_break() +
    h1("FOTOGRAF\xCDAS", 30, "_fotos") +
    '<w:p w14:paraId="F0000001" w14:textId="77777777" w:rsidR="00917BB2" w:rsidRDefault="00000000">'
    '<w:pPr><w:jc w:val="center"/></w:pPr>'
    '<w:r><w:rPr><w:noProof/></w:rPr>'
    '<w:drawing><wp:inline distT="114300" distB="114300" distL="114300" distR="114300" wp14:anchorId="38B90712" wp14:editId="2D50FDFB"><wp:extent cx="1846425" cy="2524227"/><wp:effectExtent l="0" t="0" r="0" b="0"/><wp:docPr id="9" name="image1.jpg"/><wp:cNvGraphicFramePr/><a:graphic xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"><a:graphicData uri="http://schemas.openxmlformats.org/drawingml/2006/picture"><pic:pic xmlns:pic="http://schemas.openxmlformats.org/drawingml/2006/picture"><pic:nvPicPr><pic:cNvPr id="0" name="image1.jpg"/><pic:cNvPicPr preferRelativeResize="0"/></pic:nvPicPr><pic:blipFill><a:blip r:embed="rId10"/><a:srcRect l="14452" t="9074" r="9302" b="32180"/><a:stretch><a:fillRect/></a:stretch></pic:blipFill><pic:spPr><a:xfrm><a:off x="0" y="0"/><a:ext cx="1846425" cy="2524227"/></a:xfrm><a:prstGeom prst="rect"><a:avLst/></a:prstGeom><a:ln/></pic:spPr></pic:pic></a:graphicData></a:graphic></wp:inline></w:drawing>'
    '<w:drawing><wp:inline distT="114300" distB="114300" distL="114300" distR="114300" wp14:anchorId="172C41C1" wp14:editId="17E34C1A"><wp:extent cx="1065375" cy="2442263"/><wp:effectExtent l="0" t="0" r="0" b="0"/><wp:docPr id="1" name="image9.jpg"/><wp:cNvGraphicFramePr/><a:graphic xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"><a:graphicData uri="http://schemas.openxmlformats.org/drawingml/2006/picture"><pic:pic xmlns:pic="http://schemas.openxmlformats.org/drawingml/2006/picture"><pic:nvPicPr><pic:cNvPr id="0" name="image9.jpg"/><pic:cNvPicPr preferRelativeResize="0"/></pic:nvPicPr><pic:blipFill><a:blip r:embed="rId11"/><a:srcRect/><a:stretch><a:fillRect/></a:stretch></pic:blipFill><pic:spPr><a:xfrm><a:off x="0" y="0"/><a:ext cx="1065375" cy="2442263"/></a:xfrm><a:prstGeom prst="rect"><a:avLst/></a:prstGeom><a:ln/></pic:spPr></pic:pic></a:graphicData></a:graphic></wp:inline></w:drawing>'
    '<w:drawing><wp:inline distT="114300" distB="114300" distL="114300" distR="114300" wp14:anchorId="05EB43C3" wp14:editId="2D9339CD"><wp:extent cx="1523641" cy="2490130"/><wp:effectExtent l="0" t="0" r="0" b="0"/><wp:docPr id="7" name="image6.jpg"/><wp:cNvGraphicFramePr/><a:graphic xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"><a:graphicData uri="http://schemas.openxmlformats.org/drawingml/2006/picture"><pic:pic xmlns:pic="http://schemas.openxmlformats.org/drawingml/2006/picture"><pic:nvPicPr><pic:cNvPr id="0" name="image6.jpg"/><pic:cNvPicPr preferRelativeResize="0"/></pic:nvPicPr><pic:blipFill><a:blip r:embed="rId12"/><a:srcRect/><a:stretch><a:fillRect/></a:stretch></pic:blipFill><pic:spPr><a:xfrm><a:off x="0" y="0"/><a:ext cx="1523641" cy="2490130"/></a:xfrm><a:prstGeom prst="rect"><a:avLst/></a:prstGeom><a:ln/></pic:spPr></pic:pic></a:graphicData></a:graphic></wp:inline></w:drawing>'
    '<w:drawing><wp:inline distT="114300" distB="114300" distL="114300" distR="114300" wp14:anchorId="041D3C87" wp14:editId="70A7AC5B"><wp:extent cx="1398750" cy="2473152"/><wp:effectExtent l="0" t="0" r="0" b="0"/><wp:docPr id="8" name="image11.jpg"/><wp:cNvGraphicFramePr/><a:graphic xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"><a:graphicData uri="http://schemas.openxmlformats.org/drawingml/2006/picture"><pic:pic xmlns:pic="http://schemas.openxmlformats.org/drawingml/2006/picture"><pic:nvPicPr><pic:cNvPr id="0" name="image11.jpg"/><pic:cNvPicPr preferRelativeResize="0"/></pic:nvPicPr><pic:blipFill><a:blip r:embed="rId13"/><a:srcRect/><a:stretch><a:fillRect/></a:stretch></pic:blipFill><pic:spPr><a:xfrm><a:off x="0" y="0"/><a:ext cx="1398750" cy="2473152"/></a:xfrm><a:prstGeom prst="rect"><a:avLst/></a:prstGeom><a:ln/></pic:spPr></pic:pic></a:graphicData></a:graphic></wp:inline></w:drawing>'
    '<w:drawing><wp:inline distT="114300" distB="114300" distL="114300" distR="114300" wp14:anchorId="6B8C0A6E" wp14:editId="76790309"><wp:extent cx="1089188" cy="2512000"/><wp:effectExtent l="0" t="0" r="0" b="0"/><wp:docPr id="5" name="image5.jpg"/><wp:cNvGraphicFramePr/><a:graphic xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"><a:graphicData uri="http://schemas.openxmlformats.org/drawingml/2006/picture"><pic:pic xmlns:pic="http://schemas.openxmlformats.org/drawingml/2006/picture"><pic:nvPicPr><pic:cNvPr id="0" name="image5.jpg"/><pic:cNvPicPr preferRelativeResize="0"/></pic:nvPicPr><pic:blipFill><a:blip r:embed="rId14"/><a:srcRect/><a:stretch><a:fillRect/></a:stretch></pic:blipFill><pic:spPr><a:xfrm><a:off x="0" y="0"/><a:ext cx="1089188" cy="2512000"/></a:xfrm><a:prstGeom prst="rect"><a:avLst/></a:prstGeom><a:ln/></pic:spPr></pic:pic></a:graphicData></a:graphic></wp:inline></w:drawing>'
    '<w:drawing><wp:inline distT="114300" distB="114300" distL="114300" distR="114300" wp14:anchorId="1B7671D3" wp14:editId="29F96161"><wp:extent cx="1089188" cy="2496911"/><wp:effectExtent l="0" t="0" r="0" b="0"/><wp:docPr id="3" name="image4.jpg"/><wp:cNvGraphicFramePr/><a:graphic xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"><a:graphicData uri="http://schemas.openxmlformats.org/drawingml/2006/picture"><pic:pic xmlns:pic="http://schemas.openxmlformats.org/drawingml/2006/picture"><pic:nvPicPr><pic:cNvPr id="0" name="image4.jpg"/><pic:cNvPicPr preferRelativeResize="0"/></pic:nvPicPr><pic:blipFill><a:blip r:embed="rId15"/><a:srcRect/><a:stretch><a:fillRect/></a:stretch></pic:blipFill><pic:spPr><a:xfrm><a:off x="0" y="0"/><a:ext cx="1089188" cy="2496911"/></a:xfrm><a:prstGeom prst="rect"><a:avLst/></a:prstGeom><a:ln/></pic:spPr></pic:pic></a:graphicData></a:graphic></wp:inline></w:drawing>'
    '<w:drawing><wp:inline distT="114300" distB="114300" distL="114300" distR="114300" wp14:anchorId="5F1982C7" wp14:editId="70BB0380"><wp:extent cx="4308638" cy="2439720"/><wp:effectExtent l="0" t="0" r="0" b="0"/><wp:docPr id="4" name="image12.jpg"/><wp:cNvGraphicFramePr/><a:graphic xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"><a:graphicData uri="http://schemas.openxmlformats.org/drawingml/2006/picture"><pic:pic xmlns:pic="http://schemas.openxmlformats.org/drawingml/2006/picture"><pic:nvPicPr><pic:cNvPr id="0" name="image12.jpg"/><pic:cNvPicPr preferRelativeResize="0"/></pic:nvPicPr><pic:blipFill><a:blip r:embed="rId16"/><a:srcRect/><a:stretch><a:fillRect/></a:stretch></pic:blipFill><pic:spPr><a:xfrm><a:off x="0" y="0"/><a:ext cx="4308638" cy="2439720"/></a:xfrm><a:prstGeom prst="rect"><a:avLst/></a:prstGeom><a:ln/></pic:spPr></pic:pic></a:graphicData></a:graphic></wp:inline></w:drawing>'
    '<w:drawing><wp:inline distT="114300" distB="114300" distL="114300" distR="114300" wp14:anchorId="51824554" wp14:editId="0DAA7CDD"><wp:extent cx="1300935" cy="2989994"/><wp:effectExtent l="0" t="0" r="0" b="0"/><wp:docPr id="12" name="image8.jpg"/><wp:cNvGraphicFramePr/><a:graphic xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"><a:graphicData uri="http://schemas.openxmlformats.org/drawingml/2006/picture"><pic:pic xmlns:pic="http://schemas.openxmlformats.org/drawingml/2006/picture"><pic:nvPicPr><pic:cNvPr id="0" name="image8.jpg"/><pic:cNvPicPr preferRelativeResize="0"/></pic:nvPicPr><pic:blipFill><a:blip r:embed="rId17"/><a:srcRect/><a:stretch><a:fillRect/></a:stretch></pic:blipFill><pic:spPr><a:xfrm><a:off x="0" y="0"/><a:ext cx="1300935" cy="2989994"/></a:xfrm><a:prstGeom prst="rect"><a:avLst/></a:prstGeom><a:ln/></pic:spPr></pic:pic></a:graphicData></a:graphic></wp:inline></w:drawing>'
    '</w:r></w:p>\n'
)

SECT_PR = (
    '<w:sectPr w:rsidR="00917BB2">'
    '<w:footerReference w:type="default" r:id="rId23"/>'
    '<w:headerReference w:type="first" r:id="rId24"/>'
    '<w:footerReference w:type="first" r:id="rId25"/>'
    '<w:pgSz w:w="11909" w:h="16834"/>'
    '<w:pgMar w:top="1440" w:right="1440" w:bottom="1440" w:left="1440" w:header="720" w:footer="720" w:gutter="0"/>'
    '<w:pgNumType w:start="0"/>'
    '<w:cols w:space="720"/>'
    '<w:titlePg/>'
    '</w:sectPr>'
)

# ── Assemble document ──────────────────────────────────────────────────────

NS = (
    '<?xml version="1.0" encoding="UTF-8"?>'
    '<w:document xmlns:wpc="http://schemas.microsoft.com/office/word/2010/wordprocessingCanvas" '
    'xmlns:cx="http://schemas.microsoft.com/office/drawing/2014/chartex" '
    'xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006" '
    'xmlns:aink="http://schemas.microsoft.com/office/drawing/2016/ink" '
    'xmlns:o="urn:schemas-microsoft-com:office:office" '
    'xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships" '
    'xmlns:m="http://schemas.openxmlformats.org/officeDocument/2006/math" '
    'xmlns:v="urn:schemas-microsoft-com:vml" '
    'xmlns:wp14="http://schemas.microsoft.com/office/word/2010/wordprocessingDrawing" '
    'xmlns:wp="http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing" '
    'xmlns:w10="urn:schemas-microsoft-com:office:word" '
    'xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main" '
    'xmlns:w14="http://schemas.microsoft.com/office/word/2010/wordml" '
    'xmlns:w15="http://schemas.microsoft.com/office/word/2012/wordml" '
    'xmlns:w16cex="http://schemas.microsoft.com/office/word/2018/wordml/cex" '
    'xmlns:w16cid="http://schemas.microsoft.com/office/word/2016/wordml/cid" '
    'xmlns:w16="http://schemas.microsoft.com/office/word/2018/wordml" '
    'xmlns:w16sdtdh="http://schemas.microsoft.com/office/word/2020/wordml/sdtdatahash" '
    'xmlns:w16se="http://schemas.microsoft.com/office/word/2015/wordml/symex" '
    'xmlns:wpg="http://schemas.microsoft.com/office/word/2010/wordprocessingGroup" '
    'xmlns:wpi="http://schemas.microsoft.com/office/word/2010/wordprocessingInk" '
    'xmlns:wne="http://schemas.microsoft.com/office/word/2006/wordml" '
    'xmlns:wps="http://schemas.microsoft.com/office/word/2010/wordprocessingShape" '
    'mc:Ignorable="w14 w15 w16se w16cid w16 w16cex w16sdtdh wp14">'
)

BODY = (
    COVER +
    TOC_HEADER + TOC_BLOCK + empty() + page_break() +
    RESUMEN + INTEGRANTES + IMPLEMENTACION +
    EVALUACION + APP_MOVIL + ESQ_ELEC + ESQ_FLU +
    ANEXOS + REFERENCIAS + FOTOS +
    SECT_PR
)

DOC_XML = NS + '<w:body>' + BODY + '</w:body></w:document>'

# Write document.xml
doc_path = os.path.join(BASE, 'word', 'document.xml')
with open(doc_path, 'w', encoding='utf-8') as f:
    f.write(DOC_XML)
print(f"document.xml escrito: {len(DOC_XML):,} caracteres")

# Pack as DOCX
print(f"Empaquetando {BASE} -> {OUTPUT}")
with zipfile.ZipFile(OUTPUT, 'w', zipfile.ZIP_DEFLATED) as z:
    for root, dirs, files in os.walk(BASE):
        for fname in files:
            fpath = os.path.join(root, fname)
            arcname = os.path.relpath(fpath, BASE).replace('\\', '/')
            z.write(fpath, arcname)
print(f"INFORME_DESTINO.docx generado: {os.path.getsize(OUTPUT):,} bytes")
