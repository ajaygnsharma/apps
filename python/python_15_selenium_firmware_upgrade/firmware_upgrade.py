from selenium import webdriver
from selenium.webdriver.common.by import By

# Path to the GeckoDriver executable (e.g., geckodriver for Firefox)
file_path = '/home/asharma/PycharmProjects/python_15_selenium_firmware_upgrade/ibb-v1.03.swu'
website_url = 'https://10.10.12.36:8080'

# Initialize the WebDriver (e.g., for Chrome)
cService = webdriver.ChromeService(executable_path='/home/asharma/bin/chrome-linux64/chrome')
driver = webdriver.Chrome(service = cService)

#driver = webdriver.Chrome('/home/asharma/bin/chrome-linux64/chrome')

#options = webdriver.ChromeOptions()
#options.binary_location = '/home/asharma/bin/chrome-linux64/chrome'

# Open the website
driver.get(website_url)

# Find the file input element on the webpage (you may need to inspect the webpage to find the correct selector)
file_input = driver.find_element(By.XPATH, '//input[@type="file"]')

# Send the file path to the file input element
file_input.send_keys(file_path)

# Optionally, you can click a button to submit the form after uploading the file
# upload_button = driver.find_element(By.XPATH, '//button[@type="submit"]')
# upload_button.click()

# Close the WebDriver
driver.quit()
