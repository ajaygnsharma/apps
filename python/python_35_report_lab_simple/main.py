from reportlab.lib.pagesizes import A4
from reportlab.pdfgen import canvas

c = canvas.Canvas("example.pdf", pagesize=A4)
c.setFont("Helvetica", 12)
c.drawString(50, 800, "Hello, world!")
c.showPage()
c.save()
