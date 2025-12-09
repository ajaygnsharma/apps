import requests
from bs4 import BeautifulSoup
import urllib3

urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)
from bs4 import BeautifulSoup

# Disable SSL verification (you can remove this if using a trusted cert)
url = 'https://10.10.12.39/'
r = requests.get(url, verify=False)

soup = BeautifulSoup(r.text, 'html.parser')

# Find the form
form = soup.find('form')
action = form.get('action') if form else 'index.html'  # fallback if not found

# Find input fields
form_data = {}
for input_tag in form.find_all('input'):
    input_type = input_tag.get('type', '')
    name = input_tag.get('name')
    if name:
        form_data[name] = ''  # initialize with empty value

print("Form action:", action)
print("Form fields detected:", form_data)


# Fill in the actual credentials
form_data['username'] = 'admin'
form_data['passwd'] = 'tomatosauce1234567890'

# Construct full POST URL (resolve relative path if needed)
post_url = url.rstrip('/') + '/' + action.lstrip('/')

# Send POST request
post_response = requests.post(post_url, data=form_data, verify=False)

# Print result or parse with BeautifulSoup again
print("\nPOST Response:")
print(post_response.status_code)
print(post_response.text[:10000])  # print first 1000 chars

soup = BeautifulSoup(post_response.text, 'html.parser')

# Find all <tr> rows inside the table body
rows = soup.select('table tbody tr')

# Parse as dictionary
info = {}
for row in rows:
    cols = row.find_all('td')
    if len(cols) == 2:
        key = cols[0].text.strip()
        value = cols[1].text.strip()
        info[key] = value

# Example: access specific fields
print("Spectral Inversion:", info.get("Spectral Inversion"))
print("Serial Number:", info.get("Serial Number"))

# Or print everything
print("\nExtracted Info:")
for k, v in info.items():
    print(f"{k}: {v}")