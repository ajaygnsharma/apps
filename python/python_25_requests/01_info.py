import requests
import urllib3
urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)
from bs4 import BeautifulSoup

r = requests.get('https://10.10.12.39/', verify=False)
#print(r.status_code, r.text)


# Parse the HTML
soup = BeautifulSoup(r.text, 'html.parser')

# Extract the textarea message
notice = soup.find('textarea', {'id': 'notice'})
if notice:
    print("Notice Message:")
    print(notice.text.strip())

# Extract labels like 'User ID' and 'Password'
labels = soup.find_all('label')
print("\nLabels Found:")
for label in labels:
    print("-", label.text.strip())
