QT         += network
CONFIG     += console
HEADERS     = myqtapp.h mythread.h
SOURCES	    = myqtapp.cpp main.cpp mythread.cpp
FORMS       = myqtapp.ui


# install
target.path = myqtapp
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro
sources.path = .
INSTALLS += target sources

