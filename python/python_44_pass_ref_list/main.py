

def addmember(name, age, members_list) -> list:
    members_list.append({"name": name, "age": age})
    return members_list


if __name__ == "__main__":
    members = []
    members = addmember("Alice", 30, members)
    members = addmember("Bob", 25, members)
    print(members)
