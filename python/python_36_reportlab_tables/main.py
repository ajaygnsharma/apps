from reportlab.platypus import SimpleDocTemplate, Paragraph, Table
from reportlab.lib.styles import getSampleStyleSheet

doc = SimpleDocTemplate("commands.pdf")
styles = getSampleStyleSheet()

elements = []
elements.append(Paragraph("Command Reference", styles["Title"]))
elements.append(Paragraph("C10? – Get 10MHz Detector State", styles["Normal"]))

doc.build(elements)
