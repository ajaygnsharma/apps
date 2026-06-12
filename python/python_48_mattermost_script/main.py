import requests

requests.post(
    "http://tci-dbl01.local:8065/hooks/ztdagzmma7nkpmybxiq1h1jrrc",
    json={
      "channel": "off-topic",
      "text": "LNB alarm detected"
    }
)