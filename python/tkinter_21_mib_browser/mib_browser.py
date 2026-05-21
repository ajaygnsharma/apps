#!/usr/bin/env python3
"""
MIB Browser - A Python-based SNMP MIB Browser
Similar to iReasoning MIB Browser
"""

import sys
import os
import re
from pathlib import Path
from datetime import datetime

from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QSplitter, QTreeWidget, QTreeWidgetItem, QTableWidget, QTableWidgetItem,
    QMenuBar, QMenu, QToolBar, QStatusBar, QDialog, QDialogButtonBox,
    QFormLayout, QLineEdit, QComboBox, QPushButton, QLabel, QFileDialog,
    QMessageBox, QInputDialog, QProgressDialog, QTabWidget, QTextEdit,
    QHeaderView, QCheckBox, QGroupBox, QSpinBox
)
from PyQt6.QtCore import Qt, QTimer, QThread, pyqtSignal, QSettings
from PyQt6.QtGui import QAction, QIcon, QFont, QColor

# SNMP
from easysnmp import Session
from easysnmp.exceptions import (
    EasySNMPError, EasySNMPTimeoutError, EasySNMPConnectionError,
    EasySNMPNoSuchObjectError, EasySNMPNoSuchInstanceError
)


# ============ MIB PARSER & LOADER ============

class SimpleMibParser:
    """Regex-based ASN.1/SMI MIB parser — extracts object names and numeric OIDs."""

    # Base OIDs that virtually every MIB builds on top of
    WELL_KNOWN = {
        'ccitt':           '0',
        'iso':             '1',
        'joint-iso-ccitt': '2',
        'org':             '1.3',
        'dod':             '1.3.6',
        'internet':        '1.3.6.1',
        'directory':       '1.3.6.1.1',
        'mgmt':            '1.3.6.1.2',
        'mib-2':           '1.3.6.1.2.1',
        'transmission':    '1.3.6.1.2.1.10',
        'experimental':    '1.3.6.1.3',
        'private':         '1.3.6.1.4',
        'enterprises':     '1.3.6.1.4.1',
        'security':        '1.3.6.1.5',
        'snmpV2':          '1.3.6.1.6',
        'snmpModules':     '1.3.6.1.6.3',
        # Common sub-trees
        'system':          '1.3.6.1.2.1.1',
        'interfaces':      '1.3.6.1.2.1.2',
        'at':              '1.3.6.1.2.1.3',
        'ip':              '1.3.6.1.2.1.4',
        'icmp':            '1.3.6.1.2.1.5',
        'tcp':             '1.3.6.1.2.1.6',
        'udp':             '1.3.6.1.2.1.7',
        'egp':             '1.3.6.1.2.1.8',
        'snmp':            '1.3.6.1.2.1.11',
        'ifMIB':           '1.3.6.1.2.1.31',
        'ifMIBObjects':    '1.3.6.1.2.1.31.1',
        'hrSystem':        '1.3.6.1.2.1.25.1',
        'hrStorage':       '1.3.6.1.2.1.25.2',
        'hrDevice':        '1.3.6.1.2.1.25.3',
    }

    # Matches: NAME  KEYWORD ... ::= { parent-name  index }
    # The non-greedy .*? stops at the first ::= after the keyword.
    # Quoted strings are stripped before matching so DESCRIPTION text can't confuse it.
    _ASSIGN_RE = re.compile(
        r'([A-Za-z][\w-]*)\s+'
        r'(?:OBJECT\s+IDENTIFIER|OBJECT-TYPE|MODULE-IDENTITY|MODULE-COMPLIANCE'
        r'|OBJECT-GROUP|NOTIFICATION-TYPE|NOTIFICATION-GROUP'
        r'|TEXTUAL-CONVENTION|AGENT-CAPABILITIES|TRAP-TYPE)'
        r'.*?'
        r'::=\s*\{\s*([A-Za-z][\w-]*)\s+(\d+)\s*\}',
        re.DOTALL,
    )

    def parse_file(self, filepath):
        """Return (mib_name, [{'name': str, 'oid': str}, ...]) sorted by OID."""
        mib_name = os.path.splitext(os.path.basename(filepath))[0]
        try:
            with open(filepath, encoding='utf-8', errors='replace') as fh:
                raw = fh.read()
        except OSError:
            return mib_name, []

        # Strip ASN.1 line comments, then replace quoted strings with ""
        # so text inside DESCRIPTION/SYNTAX cannot match our pattern.
        text = re.sub(r'--[^\n]*', '', raw)
        text = re.sub(r'"[^"]*"', '""', text, flags=re.DOTALL)

        # Collect name -> (parent, index) pairs
        nodes = {}
        for m in self._ASSIGN_RE.finditer(text):
            name, parent, idx = m.group(1), m.group(2), int(m.group(3))
            nodes[name] = (parent, idx)

        # Resolve full numeric OIDs by iterating until no more parents can be resolved
        known = dict(self.WELL_KNOWN)
        changed = True
        while changed:
            changed = False
            for name, (parent, idx) in nodes.items():
                if name not in known and parent in known:
                    known[name] = f"{known[parent]}.{idx}"
                    changed = True

        result = [
            {'name': name, 'oid': known[name]}
            for name in nodes
            if name in known
        ]
        result.sort(key=lambda x: [int(p) for p in x['oid'].split('.')])
        return mib_name, result


class MibLoader:
    """Parses MIB files and makes their OID trees available to the browser."""

    def __init__(self):
        self.parser  = SimpleMibParser()
        self.modules = []   # list of (mib_name, [{'name', 'oid'}, ...])

    def load_mibs(self, mib_paths=None):
        """Parse each MIB file; return (loaded_paths, error_strings)."""
        loaded = []
        errors = []

        for mib_path in (mib_paths or []):
            try:
                mib_name, objects = self.parser.parse_file(mib_path)
                if not objects:
                    errors.append(f"{mib_path}: no resolvable OIDs found")
                else:
                    self.modules.append((mib_name, objects))
                    loaded.append(mib_path)
            except Exception as e:
                errors.append(f"{mib_path}: {e}")

        return loaded, errors

    def get_oid_tree(self):
        """Return parsed modules as [{'name', 'type', 'children'}, ...]."""
        tree = []
        for mib_name, objects in self.modules:
            tree.append({
                'name': mib_name,
                'type': 'module',
                'children': [
                    {'name': obj['name'], 'oid': obj['oid'], 'type': 'object'}
                    for obj in objects
                ],
            })
        return tree

    def resolve_oid(self, oid_or_name):
        return oid_or_name, oid_or_name


# ============ SNMP OPERATOR ============

class SnmpOperator(QThread):
    """Background SNMP operations using easysnmp"""

    finished = pyqtSignal(list)
    error    = pyqtSignal(str)
    progress = pyqtSignal(int, int)

    def __init__(self, host, community, version, operation, oids, port=161):
        super().__init__()
        self.host      = host
        self.community = community
        self.version   = version
        self.operation = operation
        self.oids      = oids
        self.port      = port

    # ------------------------------------------------------------------
    # helpers
    # ------------------------------------------------------------------

    @staticmethod
    def _full_oid(var):
        """Return the complete OID string from an SNMPVariable."""
        if var.oid_index:
            return f"{var.oid}.{var.oid_index}"
        return var.oid

    @staticmethod
    def _var_to_result(var):
        return {
            'oid':   SnmpOperator._full_oid(var),
            'value': var.value,
            'type':  var.snmp_type,
        }

    # ------------------------------------------------------------------
    # thread entry point
    # ------------------------------------------------------------------

    def run(self):
        try:
            version_map = {'1': 1, '2c': 2, '3': 3}
            snmp_version = version_map.get(self.version, 2)

            session = Session(
                hostname    = self.host,
                community   = self.community,
                version     = snmp_version,
                remote_port = self.port,
                timeout     = 5,
                retries     = 3,
            )

            results = []
            total   = len(self.oids)

            # ---- GET ----
            if self.operation == 'get':
                for i, oid in enumerate(self.oids):
                    self.progress.emit(i + 1, total)
                    try:
                        var = session.get(oid)
                        results.append(self._var_to_result(var))
                    except (EasySNMPNoSuchObjectError,
                            EasySNMPNoSuchInstanceError) as e:
                        results.append({'oid': oid, 'error': str(e)})
                    except EasySNMPError as e:
                        results.append({'oid': oid, 'error': str(e)})

            # ---- GET NEXT ----
            elif self.operation == 'next':
                for i, oid in enumerate(self.oids):
                    self.progress.emit(i + 1, total)
                    try:
                        var = session.get_next(oid)
                        results.append(self._var_to_result(var))
                    except EasySNMPError as e:
                        results.append({'oid': oid, 'error': str(e)})

            # ---- WALK ----
            elif self.operation == 'walk':
                for i, oid in enumerate(self.oids):
                    self.progress.emit(i + 1, total)
                    try:
                        for var in session.walk(oid):
                            results.append(self._var_to_result(var))
                    except EasySNMPError as e:
                        results.append({'oid': oid, 'error': str(e)})

            # ---- BULK WALK ----
            elif self.operation == 'bulkwalk':
                for i, oid in enumerate(self.oids):
                    self.progress.emit(i + 1, total)
                    try:
                        for var in session.bulkwalk(oid):
                            results.append(self._var_to_result(var))
                    except EasySNMPError as e:
                        results.append({'oid': oid, 'error': str(e)})

            # ---- SET ----
            elif self.operation == 'set':
                for i, oid_val in enumerate(self.oids):
                    self.progress.emit(i + 1, total)
                    if '=' not in oid_val:
                        results.append({'oid': oid_val, 'error': 'No value specified'})
                        continue
                    oid, value = oid_val.split('=', 1)
                    try:
                        session.set(oid.strip(), value.strip())
                        results.append({'oid': oid.strip(), 'value': value.strip(), 'type': 'OCTET_STR'})
                    except EasySNMPError as e:
                        results.append({'oid': oid.strip(), 'error': str(e)})

            self.finished.emit(results)

        except EasySNMPTimeoutError:
            self.error.emit(f"Timeout connecting to {self.host}:{self.port}")
        except EasySNMPConnectionError as e:
            self.error.emit(f"Connection error: {e}")
        except Exception as e:
            self.error.emit(str(e))


# ============ MAIN WINDOW ============

class MibBrowserWindow(QMainWindow):
    """Main MIB Browser Window"""

    def __init__(self):
        super().__init__()
        self.mib_loader   = MibLoader()
        self.settings     = QSettings('MIBBrowser', 'PythonMIBBrowser')
        self.current_host = None
        self.current_oids = []

        self.init_ui()
        self.load_settings()
        QTimer.singleShot(500, self.load_default_mibs)

    def init_ui(self):
        """Initialize the user interface"""
        self.setWindowTitle("MIB Browser - Python SNMP Manager")
        self.setGeometry(100, 100, 1200, 700)

        self.create_menu_bar()
        self.create_toolbar()

        central = QWidget()
        self.setCentralWidget(central)

        main_layout = QVBoxLayout(central)

        conn_bar = self.create_connection_bar()
        main_layout.addLayout(conn_bar)

        splitter = QSplitter(Qt.Orientation.Horizontal)

        # Left: MIB Tree
        self.mib_tree = QTreeWidget()
        self.mib_tree.setHeaderLabel("MIB Structure")
        self.mib_tree.itemDoubleClicked.connect(self.on_tree_item_double_click)
        self.mib_tree.setContextMenuPolicy(Qt.ContextMenuPolicy.CustomContextMenu)
        self.mib_tree.customContextMenuRequested.connect(self.show_tree_context_menu)
        splitter.addWidget(self.mib_tree)

        # Right: Results area
        right_widget = QWidget()
        right_layout = QVBoxLayout(right_widget)

        ops_bar = self.create_operations_bar()
        right_layout.addLayout(ops_bar)

        self.results_tabs = QTabWidget()

        self.results_table = QTableWidget()
        self.results_table.setColumnCount(4)
        self.results_table.setHorizontalHeaderLabels(['OID', 'Name', 'Type', 'Value'])
        self.results_table.horizontalHeader().setStretchLastSection(True)
        self.results_table.setAlternatingRowColors(True)
        self.results_tabs.addTab(self.results_table, "Results")

        self.raw_view = QTextEdit()
        self.raw_view.setReadOnly(True)
        self.raw_view.setFont(QFont("Courier", 9))
        self.results_tabs.addTab(self.raw_view, "Raw")

        right_layout.addWidget(self.results_tabs)
        splitter.addWidget(right_widget)

        splitter.setStretchFactor(0, 1)
        splitter.setStretchFactor(1, 2)

        main_layout.addWidget(splitter)

        self.status_bar = QStatusBar()
        self.setStatusBar(self.status_bar)
        self.status_bar.showMessage("Ready")

    def create_menu_bar(self):
        """Create menu bar"""
        menubar = self.menuBar()

        file_menu = menubar.addMenu("&File")

        load_mibs_action = QAction("&Load MIBs...", self)
        load_mibs_action.setShortcut("Ctrl+O")
        load_mibs_action.triggered.connect(self.load_mibs_from_file)
        file_menu.addAction(load_mibs_action)

        file_menu.addSeparator()

        exit_action = QAction("E&xit", self)
        exit_action.setShortcut("Ctrl+Q")
        exit_action.triggered.connect(self.close)
        file_menu.addAction(exit_action)

        ops_menu = menubar.addMenu("&Operations")

        get_action = QAction("&Get", self)
        get_action.setShortcut("G")
        get_action.triggered.connect(lambda: self.perform_operation('get'))
        ops_menu.addAction(get_action)

        getnext_action = QAction("Get &Next", self)
        getnext_action.setShortcut("N")
        getnext_action.triggered.connect(lambda: self.perform_operation('next'))
        ops_menu.addAction(getnext_action)

        walk_action = QAction("&Walk", self)
        walk_action.setShortcut("W")
        walk_action.triggered.connect(lambda: self.perform_operation('walk'))
        ops_menu.addAction(walk_action)

        bulkwalk_action = QAction("&Bulk Walk", self)
        bulkwalk_action.setShortcut("B")
        bulkwalk_action.triggered.connect(lambda: self.perform_operation('bulkwalk'))
        ops_menu.addAction(bulkwalk_action)

        help_menu = menubar.addMenu("&Help")

        about_action = QAction("&About", self)
        about_action.triggered.connect(self.show_about)
        help_menu.addAction(about_action)

    def create_toolbar(self):
        """Create toolbar"""
        toolbar = QToolBar()
        self.addToolBar(toolbar)

        toolbar.addWidget(QLabel("Host:"))

        self.host_input = QLineEdit()
        self.host_input.setPlaceholderText("192.168.1.1")
        self.host_input.setMaximumWidth(150)
        toolbar.addWidget(self.host_input)

        toolbar.addWidget(QLabel("Port:"))

        self.port_input = QSpinBox()
        self.port_input.setRange(1, 65535)
        self.port_input.setValue(161)
        self.port_input.setMaximumWidth(70)
        toolbar.addWidget(self.port_input)

        toolbar.addSeparator()

        toolbar.addWidget(QLabel("Community:"))

        self.community_input = QComboBox()
        self.community_input.addItems(['public', 'private', 'manager'])
        self.community_input.setEditable(True)
        self.community_input.setMaximumWidth(100)
        toolbar.addWidget(self.community_input)

        toolbar.addWidget(QLabel("Version:"))

        self.version_input = QComboBox()
        self.version_input.addItems(['1', '2c'])
        self.version_input.setMaximumWidth(60)
        toolbar.addWidget(self.version_input)

        toolbar.addSeparator()

        self.connect_btn = QPushButton("Connect")
        self.connect_btn.clicked.connect(self.test_connection)
        toolbar.addWidget(self.connect_btn)

    def create_connection_bar(self):
        layout = QHBoxLayout()
        self.connection_status = QLabel("● Disconnected")
        self.connection_status.setStyleSheet("color: red; font-weight: bold;")
        layout.addWidget(self.connection_status)
        layout.addStretch()
        return layout

    def create_operations_bar(self):
        layout = QHBoxLayout()

        self.get_btn = QPushButton("Get")
        self.get_btn.clicked.connect(lambda: self.perform_operation('get'))
        layout.addWidget(self.get_btn)

        self.getnext_btn = QPushButton("Get Next")
        self.getnext_btn.clicked.connect(lambda: self.perform_operation('next'))
        layout.addWidget(self.getnext_btn)

        self.walk_btn = QPushButton("Walk")
        self.walk_btn.clicked.connect(lambda: self.perform_operation('walk'))
        layout.addWidget(self.walk_btn)

        self.bulkwalk_btn = QPushButton("Bulk Walk")
        self.bulkwalk_btn.clicked.connect(lambda: self.perform_operation('bulkwalk'))
        layout.addWidget(self.bulkwalk_btn)

        self.set_btn = QPushButton("Set")
        self.set_btn.clicked.connect(self.perform_set)
        layout.addWidget(self.set_btn)

        layout.addStretch()

        clear_btn = QPushButton("Clear")
        clear_btn.clicked.connect(self.clear_results)
        layout.addWidget(clear_btn)

        return layout

    def load_default_mibs(self):
        # easysnmp uses net-snmp's system MIBs automatically; just build the
        # basic tree so there is something to browse immediately.
        self.build_basic_tree()
        self.status_bar.showMessage("Ready — using built-in OID tree")

    def load_mibs_from_file(self):
        files, _ = QFileDialog.getOpenFileNames(
            self, "Select MIB Files", "", "MIB Files (*.mib *.txt);;All Files (*)"
        )

        if not files:
            return

        self.status_bar.showMessage("Parsing MIBs...")
        before = len(self.mib_loader.modules)
        loaded, errors = self.mib_loader.load_mibs(files)

        if errors:
            QMessageBox.warning(
                self, "MIB Loading Warnings",
                "Some MIBs could not be loaded:\n\n" + "\n".join(errors)
            )

        new_modules = self.mib_loader.get_oid_tree()[before:]
        for module in new_modules:
            self._add_mib_to_tree(module)

        obj_count = sum(len(m['children']) for m in new_modules)
        self.status_bar.showMessage(
            f"Loaded {len(loaded)} MIB file(s) — {obj_count} object(s) added to tree"
        )

    def _add_mib_to_tree(self, module):
        """Append a single parsed MIB module as a new top-level tree item."""
        mib_item = QTreeWidgetItem([module['name']])
        mib_item.setData(0, Qt.ItemDataRole.UserRole, {'type': 'module'})
        mib_item.setIcon(0, self.folder_icon())

        for obj in module.get('children', []):
            oid_item = QTreeWidgetItem([f"{obj['name']} ({obj['oid']})"])
            oid_item.setData(0, Qt.ItemDataRole.UserRole, {
                'type': 'object',
                'oid':  obj['oid'],
                'name': obj['name'],
            })
            oid_item.setIcon(0, self.oid_icon())
            mib_item.addChild(oid_item)

        mib_item.setExpanded(True)
        self.mib_tree.addTopLevelItem(mib_item)

    def build_basic_tree(self):
        self.mib_tree.clear()

        common_oids = {
            'SNMPv2-MIB': {
                'sysDescr':    '1.3.6.1.2.1.1.1.0',
                'sysObjectID': '1.3.6.1.2.1.1.2.0',
                'sysUpTime':   '1.3.6.1.2.1.1.3.0',
                'sysContact':  '1.3.6.1.2.1.1.4.0',
                'sysName':     '1.3.6.1.2.1.1.5.0',
                'sysLocation': '1.3.6.1.2.1.1.6.0',
                'sysServices': '1.3.6.1.2.1.1.7.0',
            },
            'IF-MIB': {
                'ifNumber':      '1.3.6.1.2.1.2.1.0',
                'ifTable':       '1.3.6.1.2.1.2.2',
                'ifIndex':       '1.3.6.1.2.1.2.2.1.1',
                'ifDescr':       '1.3.6.1.2.1.2.2.1.2',
                'ifType':        '1.3.6.1.2.1.2.2.1.3',
                'ifSpeed':       '1.3.6.1.2.1.2.2.1.5',
                'ifPhysAddress': '1.3.6.1.2.1.2.2.1.6',
                'ifAdminStatus': '1.3.6.1.2.1.2.2.1.7',
                'ifOperStatus':  '1.3.6.1.2.1.2.2.1.8',
                'ifInOctets':    '1.3.6.1.2.1.2.2.1.10',
                'ifOutOctets':   '1.3.6.1.2.1.2.2.1.16',
            },
            'TCP-MIB': {
                'tcpConnTable':        '1.3.6.1.2.1.6.13',
                'tcpConnState':        '1.3.6.1.2.1.6.13.1.1',
                'tcpConnLocalAddress': '1.3.6.1.2.1.6.13.1.2',
                'tcpConnLocalPort':    '1.3.6.1.2.1.6.13.1.3',
                'tcpConnRemAddress':   '1.3.6.1.2.1.6.13.1.4',
                'tcpConnRemPort':      '1.3.6.1.2.1.6.13.1.5',
                'tcpInSegs':           '1.3.6.1.2.1.6.5.0',
                'tcpOutSegs':          '1.3.6.1.2.1.6.6.0',
            },
            'UDP-MIB': {
                'udpTable':        '1.3.6.1.2.1.7.5',
                'udpLocalAddress': '1.3.6.1.2.1.7.5.1.1',
                'udpLocalPort':    '1.3.6.1.2.1.7.5.1.2',
                'udpInDatagrams':  '1.3.6.1.2.1.7.1.0',
                'udpNoPorts':      '1.3.6.1.2.1.7.2.0',
            },
            'IP-MIB': {
                'ipForwarding':     '1.3.6.1.2.1.4.1.0',
                'ipDefaultTTL':     '1.3.6.1.2.1.4.2.0',
                'ipInReceives':     '1.3.6.1.2.1.4.3.0',
                'ipInHdrErrors':    '1.3.6.1.2.1.4.4.0',
                'ipForwDatagrams':  '1.3.6.1.2.1.4.6.0',
                'ipInDelivers':     '1.3.6.1.2.1.4.9.0',
                'ipOutRequests':    '1.3.6.1.2.1.4.10.0',
            },
            'HOST-RESOURCES-MIB': {
                'hrSystemUptime':    '1.3.6.1.2.1.25.1.1.0',
                'hrSystemDate':      '1.3.6.1.2.1.25.1.2.0',
                'hrSystemUsers':     '1.3.6.1.2.1.25.1.5.0',
                'hrProcessorTable':  '1.3.6.1.2.1.25.3.3.1',
                'hrMemorySize':      '1.3.6.1.2.1.25.2.2.0',
            },
        }

        for mib_name, oids in common_oids.items():
            mib_item = QTreeWidgetItem([mib_name])
            mib_item.setData(0, Qt.ItemDataRole.UserRole, {'type': 'module'})
            mib_item.setIcon(0, self.folder_icon())

            for name, oid in oids.items():
                oid_item = QTreeWidgetItem([f"{name} ({oid})"])
                oid_item.setData(0, Qt.ItemDataRole.UserRole, {
                    'type': 'object',
                    'oid':  oid,
                    'name': name,
                })
                oid_item.setIcon(0, self.oid_icon())
                mib_item.addChild(oid_item)

            mib_item.setExpanded(True)
            self.mib_tree.addTopLevelItem(mib_item)

    # ------------------------------------------------------------------
    # icons
    # ------------------------------------------------------------------

    def folder_icon(self):
        try:
            return self.style().standardIcon(QMainWindow.style().StandardPixmap.SP_DirIcon)
        except Exception:
            return QIcon()

    def oid_icon(self):
        try:
            return self.style().standardIcon(QMainWindow.style().StandardPixmap.SP_FileIcon)
        except Exception:
            return QIcon()

    # ------------------------------------------------------------------
    # tree interaction
    # ------------------------------------------------------------------

    def on_tree_item_double_click(self, item, column):
        data = item.data(0, Qt.ItemDataRole.UserRole)
        if data and data.get('type') in ['object', 'column']:
            oid  = data.get('oid', '')
            name = data.get('name', '')
            self.status_bar.showMessage(f"Selected: {name} ({oid})")

    def show_tree_context_menu(self, position):
        item = self.mib_tree.itemAt(position)
        if not item:
            return
        data = item.data(0, Qt.ItemDataRole.UserRole)
        if not data:
            return

        oid  = data.get('oid', '')
        name = data.get('name', '')

        if data.get('type') not in ['object', 'column']:
            return

        menu = QMenu()
        get_action      = menu.addAction(f"Get {name}")
        walk_action     = menu.addAction(f"Walk {name}")
        menu.addSeparator()
        copy_oid_action  = menu.addAction("Copy OID")
        copy_name_action = menu.addAction("Copy Name")

        viewport = self.mib_tree.viewport()
        global_pos = viewport.mapToGlobal(position) if viewport else position
        action = menu.exec(global_pos)

        clipboard = QApplication.clipboard()
        if action == get_action:
            self.perform_operation('get', oids=[oid])
        elif action == walk_action:
            self.perform_operation('walk', oids=[oid])
        elif action == copy_oid_action and clipboard:
            clipboard.setText(oid)
        elif action == copy_name_action and clipboard:
            clipboard.setText(name)

    def get_selected_oids(self):
        oids = []
        for item in self.mib_tree.selectedItems():
            data = item.data(0, Qt.ItemDataRole.UserRole)
            if data and data.get('type') in ['object', 'column']:
                oid = data.get('oid', '')
                if oid:
                    oids.append(oid)
        return oids

    # ------------------------------------------------------------------
    # SNMP operations
    # ------------------------------------------------------------------

    def test_connection(self):
        host = self.host_input.text().strip()
        if not host:
            QMessageBox.warning(self, "Error", "Please enter a host address")
            return
        self.status_bar.showMessage(f"Testing connection to {host}...")
        self.connect_btn.setEnabled(False)
        self.perform_operation('get', oids=['1.3.6.1.2.1.1.1.0'], test=True)

    def perform_operation(self, operation, oids=None, test=False):
        host = self.host_input.text().strip()

        if not oids:
            oids = self.get_selected_oids()
        if not oids:
            QMessageBox.warning(self, "No OID Selected", "Please select an OID from the tree")
            return

        if not host:
            host = '127.0.0.1'

        community = self.community_input.currentText()
        version   = self.version_input.currentText()
        port      = self.port_input.value()

        self.connection_status.setText(f"● Connecting to {host}...")
        self.connection_status.setStyleSheet("color: orange;")
        self.set_buttons_enabled(False)

        self.snmp_op = SnmpOperator(host, community, version, operation, oids, port)
        self.snmp_op.finished.connect(lambda r: self.on_operation_complete(r, test))
        self.snmp_op.error.connect(lambda e: self.on_operation_error(e, test))
        self.snmp_op.progress.connect(self.on_progress)
        self.snmp_op.start()

    def on_operation_complete(self, results, test=False):
        self.set_buttons_enabled(True)
        host = self.host_input.text().strip() or '127.0.0.1'

        if test:
            if results and 'error' not in results[0]:
                self.connection_status.setText(f"● Connected to {host}")
                self.connection_status.setStyleSheet("color: green;")
                self.current_host = host
                self.status_bar.showMessage("Connection successful!")
            else:
                self.connection_status.setText("● Connection failed")
                self.connection_status.setStyleSheet("color: red;")
                error = results[0].get('error', 'Unknown error') if results else 'No response'
                self.status_bar.showMessage(f"Connection failed: {error}")
            self.connect_btn.setEnabled(True)
            return

        self.display_results(results)
        self.connection_status.setText(f"● Connected to {host}")
        self.connection_status.setStyleSheet("color: green;")
        self.status_bar.showMessage(f"Operation complete — {len(results)} result(s)")

    def on_operation_error(self, error, test=False):
        self.set_buttons_enabled(True)
        self.connection_status.setText("● Error")
        self.connection_status.setStyleSheet("color: red;")
        self.status_bar.showMessage(f"SNMP Error: {error}")

        if test:
            self.connect_btn.setEnabled(True)
        else:
            QMessageBox.critical(self, "SNMP Error", f"SNMP Error: {error}")

    def on_progress(self, current, total):
        self.status_bar.showMessage(f"Processing... {current}/{total}")

    def set_buttons_enabled(self, enabled):
        for btn in (self.get_btn, self.getnext_btn, self.walk_btn,
                    self.bulkwalk_btn, self.set_btn):
            btn.setEnabled(enabled)

    def perform_set(self):
        host = self.host_input.text().strip()
        if not host:
            QMessageBox.warning(self, "No Host", "Please enter a host address")
            return

        oids = self.get_selected_oids()
        if not oids:
            QMessageBox.warning(self, "No OID", "Please select an OID from the tree")
            return

        oid = oids[0]
        value, ok = QInputDialog.getText(self, "SET Operation", f"Enter value for {oid}:")
        if ok and value:
            self.status_bar.showMessage(f"Setting {oid} = {value}...")
            self.perform_operation('set', oids=[f"{oid}={value}"])

    # ------------------------------------------------------------------
    # result display
    # ------------------------------------------------------------------

    def display_results(self, results):
        self.results_table.setRowCount(0)

        raw_text  = f"SNMP Results — {datetime.now().isoformat()}\n"
        raw_text += "=" * 60 + "\n\n"

        for result in results:
            oid   = result.get('oid', '')
            value = result.get('value', '')
            error = result.get('error', '')
            stype = result.get('type', 'STRING')

            row = self.results_table.rowCount()
            self.results_table.insertRow(row)
            self.results_table.setItem(row, 0, QTableWidgetItem(oid))

            if error:
                self.results_table.setItem(row, 1, QTableWidgetItem(""))
                self.results_table.setItem(row, 2, QTableWidgetItem("ERROR"))
                self.results_table.setItem(row, 3, QTableWidgetItem(error))
                for col in range(4):
                    self.results_table.item(row, col).setBackground(QColor(255, 200, 200))
                raw_text += f"ERROR: {oid} — {error}\n"
            else:
                self.results_table.setItem(row, 1, QTableWidgetItem(""))
                self.results_table.setItem(row, 2, QTableWidgetItem(stype))
                self.results_table.setItem(row, 3, QTableWidgetItem(value))
                raw_text += f"{oid} = {value}  [{stype}]\n"

        self.raw_view.setPlainText(raw_text)
        self.results_table.resizeColumnsToContents()

    def clear_results(self):
        self.results_table.setRowCount(0)
        self.raw_view.clear()
        self.status_bar.showMessage("Results cleared")

    # ------------------------------------------------------------------
    # misc
    # ------------------------------------------------------------------

    def show_about(self):
        QMessageBox.about(self, "About MIB Browser",
            "<h3>MIB Browser</h3>"
            "<p>A Python-based SNMP MIB Browser</p>"
            "<p>Features:</p>"
            "<ul>"
            "<li>Browse built-in MIB OID tree</li>"
            "<li>Load additional MIB paths for net-snmp</li>"
            "<li>SNMP GET, GETNEXT, SET, WALK, BULKWALK operations</li>"
            "<li>Support for SNMP v1 and v2c</li>"
            "<li>Results table and raw view</li>"
            "</ul>"
            "<p>Built with PyQt6 and easysnmp</p>"
        )

    def load_settings(self):
        host = self.settings.value('host', '')
        if host:
            self.host_input.setText(host)
        community = self.settings.value('community', 'public')
        index = self.community_input.findText(community)
        if index >= 0:
            self.community_input.setCurrentIndex(index)

    def save_settings(self):
        self.settings.setValue('host', self.host_input.text())
        self.settings.setValue('community', self.community_input.currentText())

    def closeEvent(self, event):
        self.save_settings()
        event.accept()


# ============ MAIN ============

def main():
    app = QApplication(sys.argv)
    app.setApplicationName("MIB Browser")
    app.setApplicationVersion("1.0")

    window = MibBrowserWindow()
    window.show()

    sys.exit(app.exec())


if __name__ == '__main__':
    main()
