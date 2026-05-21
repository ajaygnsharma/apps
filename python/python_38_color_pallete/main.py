import os
from reportlab.lib import colors
from reportlab.lib.pagesizes import A4
from reportlab.platypus import SimpleDocTemplate, Table, TableStyle, Paragraph, Spacer
from reportlab.lib.styles import getSampleStyleSheet


def generate_color_palette_pdf(pdf_path="reportlab_colors.pdf"):
    # Create the PDF document with some margins
    doc = SimpleDocTemplate(
        pdf_path,
        pagesize=A4,
        leftMargin=36,
        rightMargin=36,
        topMargin=36,
        bottomMargin=36,
    )

    styles = getSampleStyleSheet()
    elements = []

    # Title
    elements.append(Paragraph("ReportLab Named Colors", styles["Title"]))
    elements.append(Spacer(1, 12))
    elements.append(Paragraph(
        "This table lists the named colors from reportlab.lib.colors.",
        styles["BodyText"],
    ))
    elements.append(Spacer(1, 12))

    # Table header
    header = ["Name", "Swatch", "RGB (0–1)", "Hex"]
    table_data = [header]

    # Table styles; we’ll add per-row swatch background styles dynamically
    table_styles = [
        ("GRID", (0, 0), (-1, -1), 0.25, colors.grey),
        ("BACKGROUND", (0, 0), (-1, 0), colors.lightgrey),
        ("ALIGN", (0, 0), (-1, 0), "CENTER"),
        ("VALIGN", (0, 0), (-1, -1), "MIDDLE"),
        ("LEFTPADDING", (0, 0), (-1, -1), 4),
        ("RIGHTPADDING", (0, 0), (-1, -1), 4),
        ("TOPPADDING", (0, 0), (-1, -1), 2),
        ("BOTTOMPADDING", (0, 0), (-1, -1), 2),
    ]

    # Collect colors and build rows
    row_idx = 1  # start after header
    for name in sorted(dir(colors)):
        if name.startswith("_"):
            continue  # skip private stuff

        val = getattr(colors, name)
        if not isinstance(val, colors.Color):
            continue

        # RGB values in 0–1 range
        rgb_str = f"{val.red:.2f}, {val.green:.2f}, {val.blue:.2f}"
        hex_str = val.hexval()  # e.g. '0xRRGGBB'

        # Data row; swatch cell (index 1) will be colored via TableStyle
        row = [name, "", rgb_str, hex_str]
        table_data.append(row)

        # Add background style for the swatch cell
        table_styles.append(
            ("BACKGROUND", (1, row_idx), (1, row_idx), val)
        )

        # Optional: draw a thin border around swatch cell
        table_styles.append(
            ("BOX", (1, row_idx), (1, row_idx), 0.25, colors.black)
        )

        row_idx += 1

    # Build the table
    table = Table(
        table_data,
        repeatRows=1,
        colWidths=[120, 60, 160, 80],  # tweak as you like
    )

    table.setStyle(TableStyle(table_styles))

    elements.append(table)

    # Generate PDF
    doc.build(elements)
    print(f"Saved color palette to: {os.path.abspath(pdf_path)}")


if __name__ == "__main__":
    generate_color_palette_pdf("reportlab_colors.pdf")
