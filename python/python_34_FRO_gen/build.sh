rm -rf docs/_build/latex/
sphinx-build -b latex docs docs/_build/latex

make -C docs/_build/latex/ LATEXMKOPTS=-silent
