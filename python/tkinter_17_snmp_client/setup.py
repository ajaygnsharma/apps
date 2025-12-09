from setuptools import setup, find_packages

setup(
    name="snmp_client",
    version="0.1",
    description="A SNMP client using easysnmp and Tkinter with custom MIB support",
    long_description="A GUI application for SNMP GET operations using easysnmp and Tkinter.",
    packages=find_packages(),
    install_requires=[
        "easysnmp",
        # list other dependencies here
    ],
    entry_points={
        "console_scripts": [
            "snmp_client = snmp_client11:main",  # ensure a main() is defined
        ]
    },
)
