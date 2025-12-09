# test_ibuc_client.py
import urllib3
import warnings

urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)
from urllib3.exceptions import InsecureRequestWarning

# Suppress only InsecureRequestWarning
warnings.simplefilter("ignore", InsecureRequestWarning)

import pytest
from ibuc_client import IBUCClient

# Fixture to reuse client in multiple tests
@pytest.fixture(scope="module")
def ibuc():
    client = IBUCClient(
        base_url="https://10.10.12.39",
        username="admin",
        password="tomatosauce1234567890"
    )
    login_response = client.login()
    assert login_response.status_code == 200
    return client

def test_0001_login_successful(ibuc):
    # Login is already asserted in fixture
    assert True


def test_0002_info_page_has_model_number(ibuc):
    info = ibuc.fetch_info()
    assert "Model Number" in info
    assert isinstance(info["Model Number"], str)
    assert info["Model Number"] == "IBI290310-4NA100UKWW-0218"

def test_0003_info_page_has_serial_number(ibuc):
    info = ibuc.fetch_info()
    assert "Serial Number" in info
    assert isinstance(info["Serial Number"], str)
    assert len(info["Serial Number"]) > 3 and len(info["Serial Number"]) < 10
    assert info["Serial Number"].startswith("TE89")

def test_0004_info_page_contains_spectral_inversion(ibuc):
    info = ibuc.fetch_info()
    assert "Spectral Inversion" in info
    assert info["Spectral Inversion"] in ["Inverted", "Non Inverted"]


def test_0005_info_page_has_firmware_version(ibuc):
    info = ibuc.fetch_info()
    assert "F/W Version" in info
    assert isinstance(info["F/W Version"], str)
    assert info["F/W Version"].startswith("FW IBB v")


def test_0006_info_page_has_hw_rev(ibuc):
    info = ibuc.fetch_info()
    assert "H/W Version" in info
    assert isinstance(info["H/W Version"], str)
    assert info["H/W Version"].startswith("00")