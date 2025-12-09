import pytest
import requests
from bs4 import BeautifulSoup

BASE_URL = "http://10.10.12.42/"  # Change this to your target IP/URL

def test_login_page_loads():
    """Check that the login page is reachable and has expected form."""
    r = requests.get(BASE_URL + 'login.html')
    assert r.status_code == 200
    soup = BeautifulSoup(r.text, "html.parser")

    form = soup.find("form", {"name": "login"})
    assert form is not None
    assert form["action"] == "/login.cgi"

    password_input = soup.find("input", {"type": "password", "name": "cpe"})
    assert password_input is not None
    assert password_input.get("maxlength") == "5"

    submit_button = soup.find("input", {"type": "submit"})
    assert submit_button is not None


info = {
    "title" : "IBR"
}

def test_login_submission():
    """Test submitting the login form with dummy password."""
    payload = {"cpe": "1234"}  # Adjust to correct parameter and test value
    r = requests.get(f"{BASE_URL}/login.cgi", params=payload)

    # Check if login was successful or expected failure
    assert r.status_code == 200  # Could be 302 if redirect happens
    assert "error" not in r.text.lower()  # Simplistic check

    print("\n=== Response Text ===")
    print(r.text)  # Shows the full HTML response (or JSON if that's what the server returns)

    # ✅ Optionally check headers
    print("\n=== Response Headers ===")
    print(r.headers)

    soup = BeautifulSoup(r.text, "html.parser")
    title = soup.find("title")
    assert title.text == info["title"]

