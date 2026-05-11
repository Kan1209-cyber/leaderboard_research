import requests
import csv

url = "https://codeforces.com/api/user.ratedList?activeOnly=false"

print("Fetching from Codeforces API, please wait...")
response = requests.get(url)
data = response.json()

users = data["result"][:100000]

with open("data/processed/users.csv", "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(["handle", "rating"])
    for user in users:
        writer.writerow([user["handle"], user["rating"]])

print(f"Done. Saved {len(users)} users to data/processed/users.csv")