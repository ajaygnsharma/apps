import requests
from bs4 import BeautifulSoup
import urllib3

urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)

class IBUCClient:
    def __init__(self, base_url, username, password):
        self.session = requests.Session()
        self.base_url = base_url.rstrip('/')
        self.username = username
        self.password = password

    def login(self):
        r = self.session.get(self.base_url, verify=False)
        soup = BeautifulSoup(r.text, 'html.parser')
        form = soup.find('form')
        action = form.get('action') if form else 'index.html'
        post_url = f"{self.base_url}/{action.lstrip('/')}"

        # Look for all names for input fields and form a dictionary
        form_data = {}
        for input_tag in form.find_all('input'):
            name = input_tag.get('name')
            if name:
                form_data[name] = ''

        # set login information
        form_data['username'] = self.username
        form_data['passwd'] = self.password

        # submit it.
        response = self.session.post(post_url, data=form_data, verify=False)
        return response

    def fetch_info(self, page='info.html'):
        info_url = f"{self.base_url}/{page}"
        r = self.session.get(info_url, verify=False)
        soup = BeautifulSoup(r.text, 'html.parser')
        rows = soup.select('table tbody tr')

        info = {}
        for row in rows:
            cols = row.find_all('td')
            if len(cols) == 2:
                key = cols[0].text.strip()
                value = cols[1].text.strip()
                info[key] = value
        return info

# Usage
if __name__ == "__main__":
    client = IBUCClient(
        base_url="https://10.10.12.39",
        username="admin",
        password="tomatosauce1234567890"
    )

    login_response = client.login()
    print("Login status:", login_response.status_code)

    info = client.fetch_info()
    print("Spectral Inversion:", info.get("Spectral Inversion"))
    print("\nAll Extracted Info:")
    for k, v in info.items():
        print(f"{k}: {v}")
