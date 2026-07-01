latex_engine = "xelatex"
project = "Release Notes for IBA/IBR/IBR"  # or set dynamically elsewhere
author  = "Ajay Sharma"
release = "0566"  # from cfg.ini (REV)
family  = "IBA/IBE/IBR"  # from cfg.ini (FAMILY)
doc_number = "FRN-23816-0BIN"

latex_elements = {
    "maketitle": r"",
    "tableofcontents": r"",
    # Keep this as a RAW string (no f/rf) so LaTeX braces are untouched
    "preamble": r"""
% --- Make geometry options stick even if Sphinx loaded it already ---
\makeatletter
\@ifpackageloaded{geometry}{
  \geometry{
    margin=10mm,
    includehead,includefoot,
    headheight=25mm,
    headsep=6mm,
    footskip=10mm
  }
}{
  \PassOptionsToPackage{
    margin=10mm,
    includehead,includefoot,
    headheight=25mm,
    headsep=6mm,
    footskip=10mm
  }{geometry}
}
\makeatother

\usepackage{graphicx}
\graphicspath{{_static/}{./}}
\usepackage{fancyhdr}

\newcommand{\CompanyName}{Terrasat Communications Inc.}
\newcommand{\CompanyAddress}{315 Digital Drive, Morgan Hill, CA 95037}
\newcommand{\CompanyYear}{\the\year}
\newcommand{\DocNumber}{FRN-23816-0BIN}
\newcommand{\DocDescription}{Release Notes for ['IBA', 'IBE/IBR']}
\newcommand{\Author}{Ajay Sharma}

\fancypagestyle{firstpage}{%
  \fancyhf{}%
  \fancyhead{%
    \noindent\makebox[\textwidth][s]{%
      \begin{minipage}[t]{0.35\textwidth}
        \includegraphics[height=14mm]{logo.png}\\[3pt]
        \textbf{\CompanyName}\\[-1pt]
        \CompanyAddress
      \end{minipage}%
      \hfill
      \begin{minipage}[t]{0.62\textwidth}
        \raggedleft\small
        \renewcommand{\arraystretch}{1.05}
        \begin{tabular}{@{} l l @{}}
          \textbf{Document Number}: & \DocNumber \\
          \textbf{Description}:     & \DocDescription \\
          \textbf{Revision}:        & \DocRevision \\
          \textbf{Date}:            & \today \\
          \textbf{Prepared By}:     & \Author \\
        \end{tabular}
      \end{minipage}%
    }%
  }%
  \fancyfoot[C]{\thepage\enspace|\enspace\DocDescription}%
  \renewcommand{\headrulewidth}{0pt}%
  \renewcommand{\footrulewidth}{0pt}%
}

% Sphinx sets \pagestyle{normal} → override that style for the rest
\fancypagestyle{normal}{%
  \fancyhf{}%
  \fancyhead[R]{\includegraphics[height=10mm]{logo.png}}%
  \fancyfoot[C]{\thepage\enspace|\enspace\DocDescription}%
  \renewcommand{\headrulewidth}{0.4pt}%
  \renewcommand{\footrulewidth}{0pt}%
}

% Ensure page 1 uses your first-page style
\AtBeginDocument{\thispagestyle{firstpage}}
""",
}

# Now append just the dynamic bits with an f-string.
# Note the doubled braces to emit literal LaTeX braces.
latex_elements["preamble"] += f"""
\\newcommand{{\\DocRevision}}{{{release}}}
\\renewcommand{{\\DocDescription}}{{Release Notes for {family}}}
"""

# If you also want Sphinx’s "project" string to show FAMILY:
project = f"Release Notes for {family}"

# (rest of conf.py)
latex_additional_files = ["_static/logo.png"]
latex_documents = [
    ("index", "ibuc_manuals.tex", project, "", "howto"),
]
