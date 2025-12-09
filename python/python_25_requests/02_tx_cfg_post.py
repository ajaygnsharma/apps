import requests

url = 'http://localhost:8080/api/data'
payload = {'key1': 'value1', 'key2': 'value2'}
headers = {'Content-Type': 'application/x-www-form-urlencoded'}  # or 'application/json'

# Send POST request
response = requests.post(url, data=payload, headers=headers)

# Validate the response
print("Status Code:", response.status_code)
if response.ok:
    print("POST successful.")
    print("Response Text:", response.text)

    # Optional: check content
    if 'expected_string' in response.text:
        print("Expected data found in response.")
    else:
        print("Expected data NOT found.")
else:
    print("POST failed with error:", response.status_code)
